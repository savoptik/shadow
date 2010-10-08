#ifndef _TCBFUNCS_H
#define _TCBFUNCS_H

#include <sys/types.h>

extern int s_drop_priv(void);
extern int s_gain_priv(void);
extern int tcb_user(const char *);
extern int tcb_create(const char *, uid_t);
extern int tcb_move(const char *, uid_t);
extern int tcb_rmdir(const char *);

#endif
