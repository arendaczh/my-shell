#ifndef _ERROR_HANDLERS_H_
#define _ERROR_HANDLERS_H_

void
handle_error(const char *);

void
handle_syntax_error();

void
handle_builtin_error(const char *);

#endif /* !_ERROR_HANDLERS_H_ */
