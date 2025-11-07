#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#include "builtins.h"
#include "config.h"
#include "utils.h"
#include "functions.h"
#include <sys/types.h>

int echo(char *[]);
int lexit(char *[]);
int lcd(char *[]);
int lkill(char *[]);
int lls(char *[]);
int undefined(char *[]);

builtin_pair builtins_table[] = {
	{"exit",	&lexit},
	{"lecho",	&echo},
	{"lcd",		&lcd},
	{"lkill",	&lkill},
	{"lls",		&lls},
	{NULL, NULL}
};

int
echo(char *argn[]) {
	int i = 1;
	if (argn[i]) printf("%s", argn[i++]);
	while (argn[i])
		printf(" %s", argn[i++]);

	printf("\n");
	fflush(stdout);
	return 0;
}

int
undefined(char *argn[]) {
	fprintf(stderr, "Command %s undefined.\n", argn[0]);
	return BUILTIN_ERROR;
}

int
lexit(char *argn[]) {
	exit(0);
}

int
lcd(char *argn[]) {
	if (argn[1] != NULL) {
		handle_builtin_error("lcd");
		return -1;
	}
	char *path;

	if (argn[0] == NULL) {
		//path to the current user’s home directory
		const char *name = "HOME";
		path = getenv(name);
	}
	else {
		path = argn[0];
	}
	if (chdir(path) == -1) {
		handle_builtin_error("lcd");
		return 1;
	}
	return 0;
}

int
lkill(char *argn[]) {

	char *arg1 = argn[0];
	char *arg2 = argn[1];
	if (arg2 == NULL) {
		arg2 = arg1;
		arg1 = NULL;
	}

	if (is_number(arg2) != 1 || (arg1 != NULL && is_number(arg1) == 0)) {
		handle_builtin_error("lkill");
		return -1;
	}

	long signal = SIGTERM;
	if (arg1 != NULL) {
		arg1 += 1;
		signal = atoi(arg1);

	}
	pid_t pid = atoi(arg2);
	if (kill(pid, signal) == -1) {
		handle_builtin_error("lkill");
		return 1;
	}
	return 0;


}

int
lls(char *argn[]) {

	DIR *dirp = opendir(".");
	struct dirent *entry;

	if (dirp == NULL) {
		handle_builtin_error("lls");
		return 1;
	}

	while (((entry = readdir(dirp)) != NULL)) {
		if (entry->d_name[0] != '.') {
			printf("%s\n", entry->d_name);
			fflush(stdout);
		}
	}

	closedir(dirp);
	return 0;

}