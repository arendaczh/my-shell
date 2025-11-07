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
#include "get_length.h"

void
handle_error(const char *message) {
	fprintf(stderr, "Error: %s\n", message);
	exit(EXIT_FAILURE);
}

void
handle_syntax_error() {
	fprintf(stderr, "%s\n", SYNTAX_ERROR_STR);
}

void
handle_builtin_error(const char *message) {
	fprintf(stderr, "Builtin %s error.\n", message);
}