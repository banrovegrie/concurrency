#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

int n, THREAD_MAX = 4; // has been genralised to n threads
int *A;

// thread control parameters
struct tsk
{
    int tsk_no, tsk_start, tsk_end;
};

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

void print()
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }

    printf("\n");
}

void check()
{
    for (int i = 1; i < n; i++)
    {
        if (A[i - 1] > A[i])
        {
            fprintf(stderr, "Doesn't Work\n");
            return;
        }
    }

    fprintf(stderr, "Works\n");
}

void merge(int start, int mid, int end)
{
    int l = mid - start + 1, r = end - mid;

    int *L = malloc(l * sizeof(int));
    int *R = malloc(r * sizeof(int));

    for (int i = 0; i < l; i++)
        L[i] = A[i + start];
    for (int i = 0; i < r; i++)
        R[i] = A[i + mid + 1];

    int x = 0, y = 0, k = start;
    while (x < l || y < r)
    {
        if (x == l)
        {
            A[k++] = R[y++];
        }
        else if (y == r)
        {
            A[k++] = L[x++];
        }
        else if (L[x] <= R[y])
        {
            A[k++] = L[x++];
        }
        else
        {
            A[k++] = R[y++];
        }
    }

    free(L);
    free(R);
}

// merge sort function
void merge_sort(int start, int end)
{
    int mid = start + (end - start) / 2;

    if (start < end)
    {
        merge_sort(start, mid);
        merge_sort(mid + 1, end);

        merge(start, mid, end);
    }
}

// thread function for multi-threading
void *mergesort(void *arg)
{
    struct tsk *tsk = arg;
    int start = tsk->tsk_start, end = tsk->tsk_end;

    int mid = start + (end - start) / 2;
    if (start < end)
    {
        merge_sort(start, mid);
        merge_sort(mid + 1, end);

        merge(start, mid, end);
    }
}

int main()
{
    clock_t BEG = clock();
    struct timespec start_time, end_time, delta;
    clock_gettime(CLOCK_REALTIME, &start_time);

    struct tsk *tsk;

    scanf("%d", &n);

    // allocate the array
    A = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &A[i]);
    }

    pthread_t threads[THREAD_MAX];
    struct tsk tsklist[THREAD_MAX];

    int len = n / THREAD_MAX, start = 0;
    for (int i = 0; i < THREAD_MAX; i++, start += len)
    {
        tsk = &tsklist[i];
        tsk->tsk_no = i;

        tsk->tsk_start = start;
        tsk->tsk_end = start + len - 1;
        if (i == (THREAD_MAX - 1))
            tsk->tsk_end = n - 1;
    }

    // creating 4 threads
    for (int i = 0; i < THREAD_MAX; i++)
    {
        tsk = &tsklist[i];
        pthread_create(&threads[i], NULL, mergesort, tsk);
    }

    // joining all 4 threads
    for (int i = 0; i < THREAD_MAX; i++)
        pthread_join(threads[i], NULL);

    // merging the final 4 parts
    struct tsk *tskm = &tsklist[0];
    for (int i = 1; i < THREAD_MAX; i++)
    {
        struct tsk *tsk = &tsklist[i];
        merge(tskm->tsk_start, tsk->tsk_start - 1, tsk->tsk_end);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    sub_timespec(start_time, end_time, &delta);

    check();
    print();

    clock_t EN = clock();
    double time_spent = (double)(EN - BEG) / CLOCKS_PER_SEC;
    fprintf(stderr, "Time: %lf\n", time_spent);
    fprintf(stderr, "Time: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);

    return 0;
}