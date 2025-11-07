#ifndef _GET_LENGTH_H_
#define _GET_LENGTH_H_

#include "config.h"
#include "siparse.h"
#include "utils.h"

//TODO: rename it
int
get_linked_list_length(argseq *);

int
get_linked_list_length_commandseq(commandseq *);

int
get_linked_list_length_redirseq(redirseq *);

int
get_linked_list_length_pipelineseq(pipelineseq *);

#endif /* !_GET_LENGTH_H_ */
