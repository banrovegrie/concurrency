#include <stdio.h>
#include <time.h>
#include <sys/time.h>

int main()
{
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    
    for (int i = 0; i < 100000000; i++);

    clock_gettime(CLOCK_REALTIME, &end_time);
    printf("%ld\n", (end_time.tv_nsec - start_time.tv_nsec));

    return 0;
}
