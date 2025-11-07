#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <sys/stat.h>

#include "config.h"
#include "siparse.h"
#include "utils.h"

int
execute_prompt(struct stat);

char **
convert_linked_list_to_string_array(argseq *, char **);

void
remove_trailing_spaces(char *);

int
is_number(const char *);

/**
 * Function redirect_fd takes two params, closes from, duplicate to to from and close to
 * @param from - file descriptor to be closed and change to resemble to
 * @param to - file descriptor to be closed and duplicated
 */
void
redirect_fd(int from, int to);

void
add_fg_proc(pid_t);

extern pid_t fg_process[MAX_LINE_LENGTH];
extern int fg_index;
extern volatile int active_size;

int
del_fg_proc(pid_t);

#endif /* !_FUNCTIONS_H_ */
