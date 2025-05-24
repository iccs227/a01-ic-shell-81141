#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include "signals.h"

extern int fg_pid;

void signal_Z(int sig_num) {
    if (fg_pid > 0) {
        kill(-fg_pid, SIGTSTP);
    }
}

void signal_C(int sig_num) {
    if (fg_pid > 0) {
        kill(-fg_pid, SIGINT);
    }
}

void setup_shell_signals(pid_t shell_pid) {
    setpgid(shell_pid, shell_pid);
    tcsetpgrp(0, shell_pid); // make shell the fg group for terminal

    //!Debugging issue 
    signal(SIGTTOU, SIG_IGN); // this is for milestone 4 
    signal(SIGTTIN, SIG_IGN);

    signal(SIGTSTP, signal_Z);
    signal(SIGINT, signal_C);
}
