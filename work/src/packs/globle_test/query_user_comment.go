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

type QueryUserCommentObj struct {
	Phone string
	Sesson_id string
}

type CommentObj struct {
	Isbn string
	Content string
}

type QueryUserCommentRespond struct {
	Message string
	Code int
	CommentList []CommentObj
}

//phone,nickname,sex,pro_photo,signature
func QueryUserComment(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&QueryUserCommentObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &QueryBookCommentRespond{}
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
		query_err := GetUserComment(db,phone,respond)
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

func GetUserComment (db *sql.DB,phone string,re *QueryBookCommentRespond) error {
	rows,db_err:=db.Query("select isbn,content from book_comment where isbn!=\"\" and phone =\""+phone+"\";")
	for rows.Next() {
		var isbn string
		var content string
		rows.Scan(&isbn,&content)
		re.CommentList = append(re.CommentList, BookCommentObj{isbn,content})
	}
	return db_err
}