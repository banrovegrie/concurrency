#include <bits/stdc++.h>
using namespace std;

int main()
{
    int n;
    cin >> n;
    cout << n << "\n";

    for (int i = 0; i < n; i++)
    {
        int x = rand()/n;
        cout << x << " ";
    }

    cout << endl;
    return 0;
}
