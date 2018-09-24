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

type UserBorrowObj struct {
	Phone string
	Sesson_id string
}

type BorrowBookObj struct{
	Book_name string
	Start_time string
	End_time string
	Book_isbn string
}

type UserBorrowRespond struct {
	Message string
	Code int
	BorrowList []BorrowBookObj
}

//phone,nickname,sex,pro_photo,signature
func GetUserBorrow(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&UserBorrowObj{}
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
	respond := &UserBorrowRespond{}
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
		query_err := UserBorrow(db,phone,respond)
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

func UserBorrow(db *sql.DB,phone string,re *UserBorrowRespond) error {
	stu_id_rows,query_err := db.Query("select stu_id from user_info where phone = \""+phone+"\"")
	var stu_id string
	if query_err != nil {
		return query_err
	}
	for stu_id_rows.Next() {
		stu_id_rows.Scan(stu_id)
	}
	rows,db_err:=db.Query("select book_name,start_time,end_time,book_isbn from book_borrow_history")
	if db_err != nil {
		return db_err
	}
	for rows.Next() {
		var borrow BorrowBookObj
		rows.Scan(&borrow.Book_name,&borrow.Start_time,&borrow.End_time,&borrow.Book_isbn)
		re.BorrowList = append(re.BorrowList,borrow)
	}
	return db_err
}