#ifndef _HANDLE_FUNCTIONS_H_

#include "config.h"
#include "siparse.h"
#include "utils.h"

void
handle_redirs(command *);

void
handle_pipeline(pipeline *);

void
handle_pipelineseq(pipelineseq *);


#endif /* !_HANDLE_FUNCTIONS_H_ */
