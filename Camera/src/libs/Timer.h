#ifndef __TIMER_H
    #define __TIMER_H

    #include <stdlib.h>
    #include <signal.h>
    #include <time.h>
    #include "log.h"

    #define TIMER_SIG1 SIGUSR1

    void set_timer(int delay_in_miliseconds, int interval_in_miliseconds, void (*callbackFunction)(int, siginfo_t *, void *));
    void stop_timer(void);

#endif