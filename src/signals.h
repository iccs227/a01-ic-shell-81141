#ifndef SIGNALS_H
#define SIGNALS_H

#include <sys/types.h>
#include <signal.h>
#include <termios.h>

void signal_Z(int sig_num);
void signal_C(int sig_num);
void setup_shell_signals(pid_t shell_pid);

#endif
