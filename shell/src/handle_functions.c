#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <bits/sigaction.h>
#include <bits/types/sigset_t.h>


#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "execute_functions.h"
#include "builtins.h"
#include "functions.h"
#include "get_length.h"
#include "error_handlers.h"



void
handle_redirs(command *com) {
	redirseq *redirs = com->redirs;
	int len = get_linked_list_length_redirseq(redirs);

	for (int i = 0; i < len; ++i) {
		redir *r = redirs->r;
		redirs = redirs->next;
		int flags = r->flags;

		int open_flags = 0;
		if (IS_RIN(flags)) {
			open_flags = O_RDONLY;
			close(0);
		}

		if (IS_RAPPEND(flags) || IS_ROUT(flags)) {
			open_flags = O_WRONLY | O_CREAT;
			if (IS_ROUT(flags)) {
				open_flags |= O_TRUNC;
			}
			else {
				open_flags |= O_APPEND;
			}
			close(1);
		}
		int fd = open(r->filename, open_flags, 0666);

		if (fd < 0) {

			if (errno == EACCES) {
				fprintf(stderr, "%s: permission denied\n", r->filename);
			}
			if (errno == ENOENT) {
				fprintf(stderr, "%s: no such file or directory\n", r->filename);
			}
			exit(EXIT_FAILURE);
		}

	}
}

void
handle_pipeline(pipeline *pipeline) {
	commandseq *commands = pipeline->commands;
	int len = get_linked_list_length_commandseq(commands);

	if (pipeline == NULL || len == 0) {
		return;
	}

	sigset_t mask, prev_mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &mask, &prev_mask) == -1) {
		handle_error("sigprocmask");
	}
	sigset_t suspend_mask;
	sigemptyset(&suspend_mask);
	sigaddset(&suspend_mask, SIGCHLD);


	int prev_in, fd[2];
	for (int i = 0; i < len; ++i) {
		command *com = commands->com;
		commands = commands->next;

		if (i < len - 1) {
			if (pipe(fd) == -1) {
				handle_error("pipe");
			}
		}
		int temp_in = fd[0], temp_out = fd[1];

		pid_t chld_pid = fork();

		if (chld_pid == 0) { //child
			if (pipeline->flags == INBACKGROUND) {
				setsid();
			}

			if (i < len - 1) {
				redirect_fd(1, temp_out);
			}

			if (i > 0) {
				redirect_fd(0, prev_in);
			}

			if (i < len - 1) {
				close(temp_in);
			}

			execute_command(com, prev_mask);
		}
		else if (chld_pid) { //parent
			if (pipeline->flags != INBACKGROUND) {
				add_fg_proc(chld_pid);
			}
			if (i > 0) {
				if (close(prev_in) == -1) {
					handle_error("close");
				}
			}
			if (i < len - 1) {
				if (close(temp_out) == -1) {
					handle_error("close");
				}
			}

			prev_in = temp_in;
		}
		else {
			handle_error("fork");
		}
	}

	if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
		handle_error("sigprocmask");
	}

	if (pipeline->flags == INBACKGROUND) {
		return;
	}

	sigset_t mask_, prev_mask_;
	sigemptyset(&mask_);
	sigaddset(&mask_, SIGCHLD);

	if (sigprocmask(SIG_BLOCK, &mask_, &prev_mask_) == -1) {
		handle_error("sigprocmask");
	}

	while (active_size > 0) {
		sigsuspend(&prev_mask_);
	}

	if (sigprocmask(SIG_SETMASK, &prev_mask_, NULL) == -1) {
		handle_error("sigprocmask");
	}


}

void
handle_pipelineseq(pipelineseq *sequence) {

	//Check if pipelineseq consists of empty commands
	pipelineseq *seq = sequence;
	int len = get_linked_list_length_pipelineseq(seq);

	pipeline *ln;
	for (int i = 0; i < len; ++i) {
		ln = seq->pipeline;
		seq = seq->next;

		int ln_len = get_linked_list_length_commandseq(ln->commands);
		if (i > 0 && ln_len == 0) {
			handle_syntax_error();
			return;
		}
		commandseq *commands = ln->commands;
		command *com;
		for (int j = 0; j < ln_len; ++j) {
			com = commands->com;
			commands = commands->next;
			if (com == NULL) {
				handle_syntax_error();
				return;
			}
		}
	}

	pipeline *ln2;
	for (int i = 0; i < len; ++i) {
		ln2 = sequence->pipeline;
		sequence = sequence->next;

		handle_pipeline(ln2);
	}



	//handle_pipeline(sequence->pipeline);
}