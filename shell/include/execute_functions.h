#ifndef _EXECUTE_FUNCTIONS_H_
#define _EXECUTE_FUNCTIONS_H_

#include "config.h"
#include "siparse.h"
#include "utils.h"


void
execute_command(command *com, sigset_t prev_mask);

//returns 1 if an argument is a shell command and 0 otherwise
int
execute_shell_command(char *);

#endif /* !_EXECUTE_FUNCTIONS_H_ */
