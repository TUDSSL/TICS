#ifndef STACK_H__
#define STACK_H__

extern char __stack;
extern char __stack_ws_restore;

#define __LD_STACK_MARKER __stack_ws_restore

#endif /* STACK_H__ */
