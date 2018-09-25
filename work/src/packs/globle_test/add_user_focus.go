//为客户端提供一个登录接口
package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"io/ioutil"
	_ "github.com/Go-SQL-Driver/MySQL"
)

type AddUserFocusObj struct {
	Phone string
	Friend_phone string
	Sesson_id string
}

type AddUserFocusRespond struct {
	Code int
	Message string
}

//phone,nickname,sex,pro_photo,signature
func AddUserFocus(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&AddUserFocusObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
    phone := request.Phone
	sesson_id := request.Sesson_id
	focus_user := request.Friend_phone

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("focus_user:",focus_user)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &AddUserFocusRespond{}
	if SessonMap[phone] == nil {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	}  else {
		_,db_err:=db.Query("insert into user_focus_user(user_x,user_y) values("+phone+","+focus_user+")")
		fmt.Printf("%s\n", "insert into user_focus_user(user_x,user_y) values("+phone+","+focus_user+")")
		if db_err != nil {
			fmt.Printf("%s\n", db_err)
			respond.Code = Code.DatabaseErr
			respond.Message = Message.DatabaseErr
		} else {
			respond.Code = Code.Success
			respond.Message = Message.Success	
		}
    }
	json_respond, json_err := json.Marshal(respond)
	if json_err != nil {
		log.Fatal(json_err)
		return
	}
	w.Write(json_respond)

}
