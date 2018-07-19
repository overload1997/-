#include<bits/stdc++.h>
const double eps=1e-3;
static int bookid;
static int userid;
static int clustnum;

struct ClUST {
    int st;
    std::set<int>clustset;//存放的是用户的ID
    void GetClustStatus() {
        int ans=0;
        int cnt[32]={0};
        for(auto iter:clustset) {
            //int bookset=book_set;
            for(int i=0;i<32;i++) {
                if((1<<i)&iter) {
                    cnt[i]++;
                }
            }
        }
        for(int i=0;i<32;i++) {
            if(cnt[i]>=clustset.size()/2) {
                ans|=1<<i;
            }
        }
        st=ans;
    }
};

struct USER {
    std::string user_name;
    int book_set;//用32位的整数代表32类书
    USER(std::string name) {
        user_name=name;
        book_set=0;
    }
};

std::vector<USER>UserArr;
std::map<std::string,int> UserId;
std::map<int,std::string> UserName;
std::vector<ClUST> ClustArr;
std::map<std::string,int> BookId;
std::map<int,int> Belong;

int CountOne(int x) { //计算二进制下1的个数
    int ans=0;
    while(x) {
        ans++;
        x&=x-1;
    }
    return ans;
}

double GetDist(int x,int y) {
    int A=x|y;
    int B=x&y;
    return CountOne(B)/sqrt(CountOne(A)+0.0);
}

void Init() {
    UserArr.clear();
    UserId.clear();
    UserName.clear();
    ClustArr.clear();
    userid=0;
    clustnum=0;
    std::cout<<"Init Finish."<<std::endl;
}

void Read_Info() { //读入文件，数据类型为 学生ID 书类号
    std::fstream file("info.txt");
    std::string username,book;
    while(file>>username>>book) {
        if(UserId.find(username)==UserId.end()) {
            UserId[username]=userid;
            UserName[userid]=username;
            UserArr.push_back(USER(username));
            userid++;
        }
        if(BookId.find(book)==BookId.end()) {
            BookId[book]=bookid++;
        }
        UserArr[UserId[username]].book_set|=(1<<BookId[book]);
    }
    clustnum=sqrt(UserArr.size());
    file.close();
    std::cout<<"Read_Info Finish."<<std::endl;
}

//聚类,获得clustnum个簇
void Get_Clust() { 
    for(int i=0;i<clustnum;i++) { //挑选clustnum个用户建立初始簇
        std::set<int>SET;
        SET.insert(i);
        ClustArr.push_back(ClUST{UserArr[i].book_set,SET});
        Belong[i]=i;
    }
    bool changeflag;
    std::cout<<userid<<" "<<clustnum<<std::endl;
    for(int itertime=0;itertime<1000;itertime++){
        //changeflag=false;
        for(int i=0;i<userid;i++) {
            int nearest_clust_id=-1;
            double nearest_clust_dis=-1;
            for(int j=0;j<clustnum;j++) {
                double dist=GetDist(UserArr[i].book_set,ClustArr[j].st);
                //std::cout<<dist<<" "<<nearest_clust_dis<<std::endl;
                if(dist>nearest_clust_dis) {
                    nearest_clust_dis=dist;
                    nearest_clust_id=j;
                }
            }
            if(Belong.find(i)!=Belong.end() && Belong[i]==nearest_clust_id ) continue;
            changeflag=true;
            ClustArr[Belong[i]].clustset.erase(i);
            ClustArr[nearest_clust_id].clustset.insert(i);
            Belong[i]=nearest_clust_id;
        }
        //std::cout<<"iter now..."<<std::endl;
        for(int i=0;i<clustnum;i++) {
            ClustArr[i].GetClustStatus();
        }
    } //while(changeflag);
    std::cout<<"Get_Clust Finish."<<std::endl;
}

void Debug() {
    for(int i=0;i<clustnum;i++) {
        std::cout<<"clust "<<i+1<<" is:"<<std::endl;
        for(auto uid:ClustArr[i].clustset) {
            std::cout<<UserName[uid]<<" ";
        }
        std::cout<<std::endl;
    }
}

int main() {
    Init();
    Read_Info();
    Get_Clust();
    Debug();
}