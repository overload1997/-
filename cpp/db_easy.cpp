#include<bits/stdc++.h>
using namespace std;
int main() {
    fstream fout("out.txt",ios::out);
    int n;
    cout<<"please input the count of parament:"<<endl;
    cin>>n;
    cout<<"please input the FormValue parament:"<<endl;
    string s[100];
    for(int i=0;i<n;i++) {
        cin>>s[i];
        fout<<s[i]<<" := r.FormValue(\""<<s[i]<<"\")"<<endl;
    }
    for(int i=0;i<n;i++) {
        string str="fmt.Println(\""+s[i]+":\","+s[i]+")";
        fout<<str<<endl;
    }
    fout<<"db, _ := sql.Open(SqlDriver, SqlSourceName)"<<endl;
    fout<<"defer db.Close()"<<endl;
	fout<<"println(\"尝试ping the 数据库\")"<<endl;
	fout<<"if err := db.Ping(); err != nil {"<<endl;
	fout<<"\tlog.Fatal(err)"<<endl;
	fout<<"\treturn"<<endl;
	fout<<"}"<<endl;
	fout<<"fmt.Println(\"连接成功\")"<<endl;
	//查询处理
    int cmd;
    cout<<"please input command -1:exit 0:query语句 1:update数据 2:add新行 3:delete某一行 :\n";
    while(cin>>cmd&&~cmd) {
        cout<<"please enter the need cnt:"<<endl;
        int need_cnt;
        cin>>need_cnt;
        vector<string> need_str;
        cout<<"please enter the need attribution:"<<endl;
        for(int i=0;i<need_cnt;i++) {
            string str;
            cin>>str;
            need_str.push_back(str);
        }
        cout<<"please enter the need table:"<<endl;
        string need_table;
        cin>>need_table;
        string total_db_string;
        string need_pd1;
        string need_pd2;
        cout<<"please enter the need pd when # stands for empty:"<<endl;
        cin>>need_pd1;
        cin>>need_pd2;
        if(cmd==0) {
            total_db_string="select ";
            for(int i=0;i<need_str.size();i++) {
                if(i) total_db_string+=",";
                total_db_string+=need_str[i];
            }
            total_db_string+=" from "+need_table;
        } else if (cmd==1) {
            cout<<"please enter the values whose count is equal to attribution:"<<endl;
            total_db_string="update "+need_table + " set ";
            for(int i=0;i<need_str.size();i++) {
                string v;
                cin>>v;
                if(i) total_db_string+=",";
                total_db_string+=need_str[i]+"=";
                total_db_string+="\"+";
                total_db_string+=v;
                total_db_string+="+\"";
            }
        } else if (cmd==2) {
            total_db_string="insert into "+need_table + "(";
            for(int i=0;i<need_str.size();i++) {
                if(i) total_db_string+=",";
                total_db_string+=need_str[i];
            }
            total_db_string+=") values(";
            cout<<"please enter the values whose count is equal to attribution:"<<endl;
            for(int i=0;i<need_str.size();i++) {
                string v;
                cin>>v;
                if(i) total_db_string+=",";
                total_db_string+="\"+";
                total_db_string+=v;
                total_db_string+="+\"";
            }
            total_db_string+=")";
        }
        if(need_pd1!="#") {
            total_db_string+=" where "+need_pd1+"\"+"+need_pd2;
        }
        string db_str=total_db_string;
        // for(int i=0;i<total_db_string.length();i++) {
        //     if(total_db_string[i]=='\"') {
        //         db_str+="\\\"";
        //     } else {
        //         db_str+=total_db_string[i];
        //     }
        // }
        if (cmd==2) {
            fout<<"db.Query(\""+db_str+"\")"<<endl;
        } else {
            fout<<"db.Query(\""+db_str+")"<<endl;
        }
        cout<<"please input command -1:exit 0:query语句 1:update数据 2:add新行 3:delete某一行 :\n";
    }
}


