#ifndef _HANDLERS
#define _HANDLERS

void
sigchld_handler();

void
init_sigchld_handler();

extern volatile int sigint;

void
sigint_handler();

void
init_sigint_handler();

void
init_handlers();

#endif //_HANDLERS
