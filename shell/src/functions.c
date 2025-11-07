#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <limits.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "functions.h"
#include "handlers.h"
#include "builtins.h"

int
execute_prompt(struct stat st) {
	if (S_ISCHR(st.st_mode)) {

		if (sigint) {
			sigint = false;
			printf("\n");
		}

		print_child_statuses();
		printf(PROMPT_STR);
		fflush(stdout);
	}
	return 1;
}

char **
convert_linked_list_to_string_array(argseq *args, char **arr) {
	char *root = args->arg;

	int size = 0;
	if (root != NULL) {
		while (size == 0 || root != args->arg) {
			arr[size] = args->arg;
			args = args->next;
			++size;
		}
	}
	arr[size] = NULL;
	return arr;
}

void
remove_trailing_spaces(char *str) {
	int len = strlen(str);

	// Start from the end of the string and move backwards
	while (len > 0 && isspace((unsigned char)str[len - 1])) {
		str[len - 1] = '\0';  // Replace the trailing space with null terminator
		len--;
	}
}

int
is_number(const char *str) {
	if (str == NULL || *str == '\0') {
		return 0;
	}
	char *end;
    long a = strtol(str, &end, 10);
	if (a > INT_MAX || a < INT_MIN || *end != '\0') return 0;
    return 1;
}

void
redirect_fd(int from, int to) {
	if (from == to) return;
	if (close(from) == -1) {
		handle_error("close");
	}
	if (dup(to) == -1) {
		handle_error("dup");
	}
	if (close(to) == -1) {
		handle_error("close");
	}

}

pid_t fg_process[MAX_LINE_LENGTH];
int fg_index = 0;
volatile int active_size = 0;

void
add_fg_proc(pid_t pid) {
	fg_process[fg_index] = pid;
	++fg_index;
	++active_size;
}

int
del_fg_proc(pid_t pid) {
	for (int i = 0; i < fg_index; ++i) {
		if (fg_process[i] == pid) {
			fg_process[i] = -1;
			--active_size;
			return 1;
		}
	}

	while(fg_index > 1 && fg_process[fg_index - 1] == -1) {
		--fg_index;
	}

	return 0;
}