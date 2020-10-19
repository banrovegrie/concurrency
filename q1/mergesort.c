#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

    int n;
    scanf("%d", &n);

    int *A = (int *)malloc(n * sizeof(int));

    for(int i = 0; i < n; i++) 
    {
        scanf("%d", &A[i]);
    }
    
    mergesort(A, 0, n);
    
    check(A, n);
    print(A, n);

    clock_t EN = clock();
    double time_spent = (double)(EN - BEG) / CLOCKS_PER_SEC;
    fprintf(stderr, "Time: %lf\n", time_spent);
        
    return 0;
}
