#include<iostream>
using namespace std;
void swapref(int &a,int &b)
{
    int t=a;
    a=b;
    b=t;
}
void swapptr(int *a,int*b)
{
    int t=*a;
    *a=*b;
    *b=t;
}
int main()
{
    int a=10,b=20;
    swapref(a,b);
    cout<<"After swapref:a="<<a<<"b="<<b<<endl;

    swapptr(&a,&b);
    cout<<"After swapptr:a="<<a<<"b="<<b<<endl;

    int &alias=a;
    alias=99;
    cout<<"a via alias="<<a<<endl;
}