#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "functions.h"
#include "builtins.h"
#include "config_2.h"
#include "error_handlers.h"
#include <bits/sigaction.h>
#include <asm-generic/signal-defs.h>

int child_pid[MAX_CHILDREN_SIZE];
int child_status[MAX_CHILDREN_SIZE];
int ix = 0;

void
print_child_statuses() {
    for (int i = 0; i < ix; ++i) {
        pid_t pid = child_pid[i];
        int status = child_status[i];
        if (WTERMSIG(status)) {
            printf("Background process %d terminated. (killed by signal %d)\n", pid, WTERMSIG(status));
            fflush(stdout);
        }
        else {
            printf("Background process %d terminated. (exited with status %d)\n", pid, WEXITSTATUS(status));
            fflush(stdout);
        }

    }
    fflush(stdout);
    ix = 0;
}

void
sigchld_handler() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (del_fg_proc(pid) == 1) { //if fg process
            continue;
        }
        child_pid[ix] = pid;
        child_status[ix] = status;
        // fprintf(stderr, "handler: %d %d\n", ix, status);
        ++ix;
    }

    //todo: think about this -> is it possible that something changes errno here?
    if (pid == -1 && errno != ECHILD) {
        _Exit(1);
    }

}

void
init_sigchld_handler() {
    struct sigaction act;
    act.sa_handler = sigchld_handler;
    act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        handle_error("sigaction");
    }
}

volatile int sigint = false;

void
sigint_handler() { 
    sigint = true;
}

void
init_sigint_handler() {
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        handle_error("sigaction");
    }
}

void
init_handlers() {
    init_sigchld_handler();
    init_sigint_handler();
}
