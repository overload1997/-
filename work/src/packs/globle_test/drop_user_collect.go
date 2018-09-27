//为客户端提供一个登录接口
package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"io/ioutil"
	_ "github.com/Go-SQL-Driver/MySQL"
)

type DropUserCollectObj struct {
	Phone string
	Isbn string
	Sesson_id string
}

type DropUserCollectRespond struct {
	Code int
	Message string
}

//phone,nickname,sex,pro_photo,signature
func DropUserCollect(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&DropUserCollectObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
    phone := request.Phone
	sesson_id := request.Sesson_id
	book_isbn := request.Isbn

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("book_isbn:",book_isbn)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        fmt.Println(err)
        return
    }
    fmt.Println("连接成功")
	respond := &DropUserCollectRespond{}
    if _,ok:=SessonMap[phone] ; !ok {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if _,ok:=SessonMap[phone]; ok&&sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if  _,ok:=SessonMap[phone]; ok && SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	}  else {
		_,db_err:=db.Query(`delete from user_collect where phone="`+phone+`" and isbn="`+book_isbn+`"`)
		fmt.Printf("%s\n", `delete from user_collect where phone="`+phone+`" and isbn="`+book_isbn+`"`)
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
		fmt.Println(json_err)
		return
	}
	w.Write(json_respond)

}
