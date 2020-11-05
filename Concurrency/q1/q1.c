#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
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

void merge(int *A, int start, int mid, int end) 
{
    int l = mid - start, r = end - mid;

    int *L = (int *) malloc(l * sizeof(int));
    int *R = (int *) malloc(r * sizeof(int));

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

    free(L);
    free(R);
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

void mergesort(int *A, int start, int end) 
{
    int mid = (start + end) / 2;
    
    //condition to not perform mergesort again
    if (start >= end - 1)
        return;
    else if (end - start <= 5)
    {
        selection_sort(A, start, end);
        return;
    }

    //create the two child processes
    pid_t lpid, rpid;
    if ((lpid = fork()) < 0)
    {
        perror("lpid");
        _exit(1);
    }
    else if (lpid == 0)
    {
        //configure left subarray
        mergesort(A, start, mid);
        _exit(0);
    }
    else
    {
        if ((rpid = fork()) < 0)
        {
            perror("rpid");
            _exit(1);
        }
        else if (rpid == 0)
        {
            //configure right subarray
            mergesort(A, mid, end);
            _exit(0);
        }
    }

    //wait for child proesses to finish
    int status;
    waitpid(lpid, &status, 0);
    waitpid(rpid, &status, 0);

    //merge the sorted subarrays
    merge(A, start, mid, end);
}

int main() 
{
    //begin clock  
    clock_t BEG = clock();
    struct timespec start_time, end_time, delta;
    clock_gettime(CLOCK_REALTIME, &start_time);

    //initiate ipc 
    int shmid;
    key_t key = IPC_PRIVATE;
    int *A;

    //enter length of array
    int n;
    scanf("%d", &n);

    //calculate segment length
    size_t SHM_SIZE = n * sizeof(int);

    //create segment
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) 
    { 
        perror("shmget"); 
        _exit(1);                    
    }

    //attach segment to our data space 
    if ((A = shmat(shmid, NULL, 0)) == (int *) -1)
    {
        perror("shmat");
        _exit(1);
    }

    //input array values
    for(int i = 0; i < n; i++) 
    {
        scanf("%d", &A[i]);
    }
    
    //call mergesort
    mergesort(A, 0, n);

    clock_gettime(CLOCK_REALTIME, &end_time);
    sub_timespec(start_time, end_time, &delta);
    
    //check if works and print
    check(A, n);
    print(A, n);

    //detach from the shared memory
    if (shmdt(A) == -1) 
    {  
        perror("shmdt"); 
        _exit(1); 
    } 

    //delete shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    { 
        perror("shmctl"); 
        _exit(1);                                     
    } 

    //mesure time passed in execution
    clock_t EN = clock();
    double time_spent = (double)(EN - BEG) / CLOCKS_PER_SEC;
    fprintf(stderr, "Time: %lf\n", time_spent);
    fprintf(stderr, "Time: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);
        
    return 0;
}
