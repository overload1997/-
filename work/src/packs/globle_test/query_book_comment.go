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

type QueryBookCommentObj struct {
	Phone string
	Isbn string
	Sesson_id string
}

type BookCommentObj struct {
	Phone string
	Content string
}

type QueryBookCommentRespond struct {
	Message string
	Code int
	CommentList []BookCommentObj
}

//phone,nickname,sex,pro_photo,signature
func QueryBookComment(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&QueryBookCommentObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
	phone := request.Phone
	isbn := request.Isbn
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
    fmt.Println("isbn:",isbn)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        fmt.Println(err)
        return
    }
    fmt.Println("连接成功")
	respond := &QueryBookCommentRespond{}
    if _,ok:=SessonMap[phone] ; !ok {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if _,ok:=SessonMap[phone]; ok&&sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if  _,ok:=SessonMap[phone]; ok && SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	} else {
		query_err := GetBookComment(db,isbn,respond)
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
		fmt.Println(json_err)
		return
	}
	w.Write(json_respond)

}

func GetBookComment (db *sql.DB,isbn string,re *QueryBookCommentRespond) error {
	rows,db_err:=db.Query("select phone,isbn from book_comment where isbn!=\"\" and isbn =\""+isbn+"\";")
	for rows.Next() {
		var phone string
		var isbn string
		rows.Scan(&phone,&isbn)
		re.CommentList = append(re.CommentList, BookCommentObj{phone,isbn})
	}
	return db_err
}