#include <iostream>
using namespace std;

int F(int x)
{
    if(x<3)
        return 1;
    else
        return F(x-1)+F(x-2);
}

int main() {
 int t,m,n;
 m=0;
 n=10;
 for(t=1;t<=n;t++) {
     m=m+F(t);
 }
 cout << m;
 return 0;
}
