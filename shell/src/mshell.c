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
#include "functions.h"
#include "execute_functions.h"
#include "handlers.h"

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2

int
main(int argc, char *argv[]) {
	init_handlers();

	pipelineseq *ln;

	struct stat st;
	if (fstat(STD_IN, &st) == -1) {
		handle_error("fstat\n");
	}

	char buff[MAX_LINE_LENGTH + 1];
	buff[MAX_LINE_LENGTH] = '\0';

	char my_buff[MAX_LINE_LENGTH + 1];
	char *my_ptr = my_buff;
	int my_len = 0;

	int result = 0;
	int len = 0;

	while (execute_prompt(st) && (result = read(STD_IN, buff, MAX_LINE_LENGTH)) > 0) {
		buff[result] = '\0';
		++result;

		char *eol = strchr(buff, '\n');
		while (eol != NULL) {
			*eol = '\0';
			len = eol - buff + 1;
			my_len += len;
			memmove(my_ptr, buff, len);

			result -= len;
			memmove(buff, eol + 1, result);

			if (my_len <= MAX_LINE_LENGTH) {
				ln = parseline(my_buff);
				if (execute_shell_command(my_buff) == 0)
					handle_pipelineseq(ln);
			}
			else {
				handle_syntax_error();
			}
			my_ptr = my_buff;
			my_len = 0;

			if (ln == NULL) {
				handle_syntax_error();
				continue;
			}
			eol = strchr(buff, '\n');
		}

		char *eof = strchr(buff, '\0');
		len = eof - buff + 1;

		result -= len;
		if ((my_ptr - my_buff) + len - 1 <= MAX_LINE_LENGTH) {
			memmove(my_ptr, buff, len - 1);
			my_ptr += len - 1;
		}
		else {
			my_ptr = my_buff;
		}
		my_len += len - 1;
	}

	return 0;
};
