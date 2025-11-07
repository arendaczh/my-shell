#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"
#include "execute_functions.h"
#include "builtins.h"
#include "functions.h"

int
get_linked_list_length(argseq *args) {
	char *root = args->arg;
	if (root == NULL) {
		return 0;
	}
	int size = 0;
	while (size == 0 || root != args->arg) {
		args = args->next;
		++size;
	}
	return size;
}

int
get_linked_list_length_commandseq(commandseq *args) {
	if (args == NULL) return 0;
	command *root = args->com;
	if (root == NULL) {
		return 0;
	}
	int size = 0;
	while ((args->com != NULL) && (size == 0 || root != args->com)) {
		args = args->next;
		++size;
	}
	return size;
}

int
get_linked_list_length_redirseq(redirseq *args) {
	if (args == NULL) return 0;
	redir *root = args->r;
	if (root == NULL) {
		return 0;
	}
	int size = 0;
	while ((args->r != NULL) && (size == 0 || root != args->r)) {
		args = args->next;
		++size;
	}
	return size;
}

int
get_linked_list_length_pipelineseq(pipelineseq *args) {
	if (args == NULL) return 0;
	pipeline *root = args->pipeline;
	if (root == NULL) {
		return 0;
	}
	int size = 0;
	while ((args->pipeline != NULL) && (size == 0 || root != args->pipeline)) {
		args = args->next;
		++size;
	}
	return size;
}


