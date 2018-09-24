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

type CommendBookObj struct {
	Phone string
	Sesson_id string
}

type CommendBookRespond struct {
	Message string
	Code int
	IsbnList []string
}

//phone,nickname,sex,pro_photo,signature
func CommendBook(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&CommendBookObj{}
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
	respond := &CommendBookRespond{}
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
		rows,exit_err := db.Query("select stu_id from user_info where phone=\""+phone+"\"")
		if exit_err != nil {
			respond.Code = Code.UnConfirmStu
			respond.Message = Message.UnConfirmStu
		} else {
			var stu_id string
			for rows.Next() {
				rows.Scan(&stu_id)
			}
			query_err := Commend(db,stu_id,respond)
			if query_err != nil {
				respond.Code = Code.DatabaseErr
				respond.Message = Message.DatabaseErr
			} else {
				respond.Code = Code.Success
				respond.Message = Message.Success
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

func HaveBorrow(stu_id string,re *CommendBookRespond) {
	db,_:=sql.Open(SqlDriver,SqlSourceName)
	defer db.Close()
	//println("尝试连接数据库......")
	if err:=db.Ping();err!=nil {
		log.Fatal(err)
		return
	}
	//println("数据库连接成功!")
	rows,query_err := db.Query("select isbn from book_borrow_history where stu_id=\""+stu_id+"\" limit 2")
	if query_err!=nil {
		log.Fatal(query_err)
		return
	}
	for rows.Next() {
		var isbn string
		rows.Scan(&isbn)
		_,ok := ReadBook[isbn]
		if ok==false {
			//fmt.Printf("%+v\n", book_name)
			re.IsbnList = append(re.IsbnList, isbn)
			ReadBook[isbn]=true
		}
		
	}
}

func Commend(db *sql.DB,stu_id string,re *CommendBookRespond) error{
	id := UserId[stu_id]
	for k:= range ReadBook {
		 delete(ReadBook, k) 
	}
	for i:=0;i<len(Farmiliar[id]);i++ {
		stud_obj := Farmiliar[id][i]
		familiar_user := UserName[stud_obj.id]
		HaveBorrow(familiar_user,re)
	}
	return nil
}