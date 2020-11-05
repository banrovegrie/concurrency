#include <stdio.h>
#include <time.h>
#include <sys/time.h>

enum
{
    NS_PER_SECOND = 1000000000
};

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

int main()
{
    struct timespec start_time, end_time, delta;
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    for (int i = 0; i < 100000000; i++);

    clock_gettime(CLOCK_REALTIME, &end_time);
    sub_timespec(start_time, end_time, &delta);

    fprintf(stderr, "Time: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);

    return 0;
}
