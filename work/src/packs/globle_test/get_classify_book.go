package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"io/ioutil"
	_ "github.com/Go-SQL-Driver/MySQL"
)

type ClassifyBook struct {
	Isbn string
	BookName string
}

type GetClassifyBookRespond struct {
	Code int
	Message string
	BookList []ClassifyBook
}

type GetClassifyBookObj struct {
	Phone string
	Type string
	K string
	Sesson_id string
}

func GetClassifyBook(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&GetClassifyBookObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
	phone := request.Phone
	book_type := request.Type
	k := request.K
	sesson_id := request.Sesson_id

    fmt.Println("book_type:",book_type)
    fmt.Println("k:",k)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        fmt.Println(err)
        return
    }
    fmt.Println("连接成功")
	respond := &TopkRespond{}
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
		query_err := GetTopk(db,book_type,k,respond)
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