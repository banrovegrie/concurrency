#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void print(int *A, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }

    printf("\n");
}

void check(int *A, int n)
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

void selection_sort(int *A, int s, int n)
{
    for (int i = s; i < n; i++)
    {
        int min = i;
        for (int j = i + 1; j < n ; j++)
        {
            if (A[j] < A[min])
                min = j;
        }
        
        //swap A[i] and A[min]
        int temp = A[i];
        A[i] = A[min];
        A[min] = temp;
    }
}

void merge(int *A, int start, int mid, int end) 
{
    int l = mid - start, r = end - mid;

    int *L = (int *)malloc(l * sizeof(int));
    int *R = (int *)malloc(r * sizeof(int));

    for (int i = 0; i < l; i++)
    {
        L[i] = A[start + i];
    }

    for (int i = 0; i < r; i++)
    {
        R[i] = A[mid + i];
    }

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
}

void mergesort(int *A, int start, int end) 
{
    int mid = (start + end) / 2;
    
    if (start >= end - 1)
        return;
    if (end - start <= 5)
    {
        selection_sort(A, start, end);
        return;
    }

    mergesort(A, start, mid);
    mergesort(A, mid, end);

    merge(A, start, mid, end);
}

int main() 
{
    clock_t BEG = clock();
    struct timespec start_time, end_time, delta;
    clock_gettime(CLOCK_REALTIME, &start_time);

    int n;
    scanf("%d", &n);

    int *A = (int *)malloc(n * sizeof(int));

    for(int i = 0; i < n; i++) 
    {
        scanf("%d", &A[i]);
    }
    
    mergesort(A, 0, n);

    clock_gettime(CLOCK_REALTIME, &end_time);
    sub_timespec(start_time, end_time, &delta);
    
    check(A, n);
    print(A, n);

    clock_t EN = clock();
    double time_spent = (double)(EN - BEG) / CLOCKS_PER_SEC;
    fprintf(stderr, "Time: %lf\n", time_spent);
    fprintf(stderr, "Time: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);

    return 0;
}
