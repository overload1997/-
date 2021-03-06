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

type AddBookCommentObj struct {
	Phone string
	Isbn string
	Content string
	Sesson_id string
}

type AddBookCommentRespond struct {
	Message string
	Code int
}

//phone,nickname,sex,pro_photo,signature
func AddBookComment(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&AddBookCommentObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	isbn := request.Isbn
	content := request.Content
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
    fmt.Println("isbn:",isbn)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &AddBookCommentRespond{}
	if SessonMap[phone] == nil {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	} else {
		query_err := AddComment(db,phone,isbn,content,respond)
		if query_err != nil {
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

func AddComment (db *sql.DB,phone string,isbn string,content string,re *AddBookCommentRespond) error {
	_,db_err:=db.Query("insert into book_comment(phone,isbn,content) values(\""+phone+"\",\""+isbn+"\",\""+content+"\");")
	return db_err
}