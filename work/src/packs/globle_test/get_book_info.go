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

type GetBookInfoObj struct {
	Phone string
	Isbn string
	Sesson_id string
}

type BookInfo struct {
	Title string
	Author string
	PublishCompany string
	PublishAddress string
	PublishDate string
	Summary string
	Type string
	Sex string
}

type GetBookInfoRespond struct {
	Code int
	Message string
	Book BookInfo
}

//phone,nickname,sex,pro_photo,signature
func GetBookInfo(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&GetBookInfoObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone 
	isbn := request.Isbn
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("isbn:",isbn)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &GetBookInfoRespond{}
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
		rows,db_err:=db.Query("select * from book_info where isbn="+isbn)
		if db_err != nil {
			respond.Code = Code.DatabaseErr
			respond.Message = Message.DatabaseErr
		} else {
			respond.Code = Code.Success
			respond.Message = Message.Success	
			for rows.Next() {
				rows.Scan(
					&respond.Book.Title,
					&respond.Book.Author,
					&respond.Book.PublishCompany,
					&respond.Book.PublishAddress,
					&respond.Book.PublishDate,
					&respond.Book.Summary,
					&respond.Book.Type,
				)
			}

		}
    }
	
	json_respond, json_err := json.Marshal(respond)
	if json_err != nil {
		log.Fatal(json_err)
		return
	}
	w.Write(json_respond)

}
