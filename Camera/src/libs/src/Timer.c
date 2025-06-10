#include "../Timer.h"

static timer_t timerID_1;

void set_timer(int delay_in_miliseconds, int interval_in_miliseconds, void (*callbackFunction)(int sig, siginfo_t *sigInfo, void *vParam))
{
    // Set up signal handler for timer
    struct sigaction sa1;
    sa1.sa_flags     = SA_SIGINFO;
    sa1.sa_sigaction = callbackFunction;
    sigemptyset(&sa1.sa_mask);
    if (sigaction(TIMER_SIG1, &sa1, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Create timer
    struct sigevent sev1;
    struct itimerspec its1;
    sev1.sigev_notify = SIGEV_SIGNAL;
    sev1.sigev_signo  = TIMER_SIG1;
    sev1.sigev_value.sival_ptr = &timerID_1;
    if (timer_create(CLOCK_REALTIME, &sev1, &timerID_1) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // Set timer expiration time and interval
    its1.it_value.tv_sec     = delay_in_miliseconds / 1000;
    its1.it_value.tv_nsec    = (delay_in_miliseconds % 1000) * 1000000;
    its1.it_interval.tv_sec  = interval_in_miliseconds / 1000;
    its1.it_interval.tv_nsec = (interval_in_miliseconds % 1000) * 1000000;

    // Start timer
    if (timer_settime(timerID_1, 0, &its1, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }
    log_info("Timer started.");
}

void stop_timer(void)
{
    timer_delete(timerID_1);
}