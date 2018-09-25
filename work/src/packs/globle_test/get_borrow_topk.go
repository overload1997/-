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

type TopkObj struct {
	Phone string
	Book_type string
	K string
	Sesson_id string
}

type TopkRespond struct {
	Message string
	Code int
	BookList []string
}

//phone,nickname,sex,pro_photo,signature
func GetBorrowTopk(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&TopkObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	book_type := request.Book_type
	k := request.K
	sesson_id := request.Sesson_id

    fmt.Println("book_type:",book_type)
    fmt.Println("k:",k)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &TopkRespond{}
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
		log.Fatal(json_err)
		return
	}
	w.Write(json_respond)

}

func GetTopk(db *sql.DB,book_type string,k string,re *TopkRespond) error {
	rows,db_err:=db.Query("select isbn from book_borrow_history,book_info where book_name!=\"\" and book_name=title and type =\""+book_type+"\" group by book_name order by count(book_name) desc limit "+k+";")
	for rows.Next() {
		var isbn string
		rows.Scan(&isbn)
		re.BookList = append(re.BookList, isbn)
	}
	return db_err
}