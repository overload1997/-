#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

ll x,y;
bool solve(ll a,ll b,ll c,ll a1,ll b1,ll c1) {
    if( !a&&!b || !a1&&!b1 || !a&&!a1 || !b&&!b1 || !(a*b)&&c%(a+b) || !(a1*b1)&&c1%(a1+b1))  return false;
    if(a&&b&&a1&&b1) {
        if(a*b1==a1*b || (a1*c-a*c1)%(a*b1-a1*b) || (b1*c-b*c1)%(b*a1-b1*a)) return false;
        x=(a1*c-a*c1)%(a*b1-a1*b),y=(b1*c-b*c1)%(b*a1-b1*a);
        return true;
    }
    if(a1*b1) {
        swap(a,a1);
        swap(b,b1);
        swap(c,c1);
    }
    if(!a) {
        y=-c/b;
        if((-c1-b1*y)%a1) return false;
        x=(-c1-b1*y)/a1;
    } 
    if(!b) {
        x=-c/a;
        if((-c1-a1*x)%b1) return false;
        x=(-c1-a1*x)/b1;
    }
    return true;
}

int main() {
    ll a,b,c,a1,b1,c1;
    while(cin>>a>>b>>c>>a1>>b1>>c1) {
        if(!solve(a,b,c,a1,b1,c1)) {
            puts("unknowned!");
        } else {
            printf("%lld %lld\n",x,y);
        }
    }
}