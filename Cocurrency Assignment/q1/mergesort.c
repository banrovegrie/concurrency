#include <stdio.h>
#include <stdlib.h>

void print(int *A, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", A[i]);
    }

    printf("\n");
}

void cheeck(int *A, int n)
{
    for (int i = 1; i < n; i++)
    {
        if (A[i - 1] > A[i])
        {
            printf("Doesn't Work\n");
            return;
        }
    }

    printf("Works\n");
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

    mergesort(A, start, mid);
    mergesort(A, mid, end);

    merge(A, start, mid, end);
}

int main() 
{
    int n;
    scanf("%d", &n);

    int *A = (int *)malloc(n * sizeof(int));

    for(int i = 0; i < n; i++) 
    {
        scanf("%d", &A[i]);
    }
    
    fprintf(stderr, "%d\n", n);
    mergesort(A, 0, n);
    
    check(A, n);
    print(A, n);

    return 0;
}
