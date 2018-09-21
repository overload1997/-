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

type AddUserCollectObj struct {
	Phone string
	Book_isbn string
	Sesson_id string
	Book_name string
}

type AddUserCollectRespond struct {
	Code int
	Message string
}

//phone,nickname,sex,pro_photo,signature
func AddUserCollect(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&AddUserCollectObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
    phone := request.Phone
	sesson_id := request.Sesson_id
	book_isbn := request.Book_isbn
	book_name := request.Book_name

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("book_isbn:",book_isbn)
	fmt.Println("book_name:",book_name)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &AddUserCollectRespond{}
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
		_,db_err:=db.Query("insert into user_collect(user, book_name,book_isbn) values("+phone+","+book_name+","+book_isbn+")")
		fmt.Printf("%s\n", "insert into user_collect(user, book_name,book_isbn) values("+phone+","+book_name+","+book_isbn+")")
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
