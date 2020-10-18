#include <bits/stdc++.h>
#include <time.h>
using namespace std;

int main()
{
    clock_t BEG = clock();

    int n;
    cin >> n;

    int *A = (int *) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        cin >> A[i];

    sort(A, A + n);
    
    clock_t EN = clock();
    double time_spent = (double)(EN - BEG) / CLOCKS_PER_SEC;
    fprintf(stderr, "Time: %lf\n", time_spent);

    return 0;
}
