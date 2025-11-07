#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "execute_functions.h"
#include "builtins.h"
#include "functions.h"
#include "get_length.h"


void
execute_command(command *com, sigset_t prev_mask) {
	argseq *args = com->args;

	int length = get_linked_list_length(args);
	char *arr[length + 1];
	convert_linked_list_to_string_array(args, arr);

	handle_redirs(com);
	if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) == -1) {
		handle_error("sigprocmask");
	}
	int result = execvp(arr[0], arr);
	if (result != -1) {
		exit(0);
	}
	if (errno == ENOENT) {
		fprintf(stderr, "%s: no such file or directory\n", arr[0]);
	}
	else if (errno == EACCES) {
		fprintf(stderr, "%s: permission denied\n", arr[0]);
	}
	else {
		fprintf(stderr, "%s: exec error\n", arr[0]);
	}
	exit(EXEC_FAILURE);
}

int
execute_shell_command(char *command) {
	remove_trailing_spaces(command);

	char *com;
	char *arg;
	char *pid;
	char *eoc = strchr(command, ' ');
	if (eoc == NULL) { ///there is no ' ', so the command does not have arguments
		com = command;
		arg = NULL;
		pid = NULL;
	}
	else {
		*eoc = '\0';
		com = command;

		command = eoc + 1;
		eoc = strchr(command, ' ');
		if (eoc == NULL) { //there is no ' ', so the command does not have the 
			arg = command;
			pid = NULL;
		}
		else {
			*eoc = '\0';
			arg = command;
			command = eoc + 1;
			pid = command;
		}
	}
	// printf("!!! %s %s %s\n", com, arg, pid);
	// fflush(stdout);


	int i = 0;

	while (i >= 0) {
		if (builtins_table[i].name == NULL) return 0;

		char *shell_command = builtins_table[i].name;
		if (strcmp(shell_command, com) == 0) {
			char *args[] = { arg, pid };
			builtins_table[i].fun(args);
			return 1;
		}
		++i;
	}

	return 0;
}
