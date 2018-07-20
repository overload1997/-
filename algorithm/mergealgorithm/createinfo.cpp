#include<bits/stdc++.h>
using namespace std;
int usernum=10000;
set<char>vis[1000005];
int main() {
        fstream file("info.txt");
        srand(time(NULL));
        int cnt=0;
        for(int i=0;i<usernum*100;i++) {
                int user=(rand()%100000);
                char book=char((rand()%26)+'A');
                if(vis[user].find(book)!=vis[user].end()) continue;
                if(vis[user].size()>=5) continue;
                vis[user].insert(book);
                //cout<<vis[user].size()<<" ";
                file<<user<<" "<<book<<endl;
                cnt++;
        }
        cout<<cnt<<endl;
}
