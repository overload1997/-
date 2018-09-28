package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"io/ioutil"
	_ "github.com/Go-SQL-Driver/MySQL"
)

type CommendClassifyBookObj struct {
	Phone string
	Sesson_id string
}

type ClassifyBookList struct {
	Isbn string
	BorrowCount int
}

type CommendClassifyBookRespond struct {
	Message string
	Code int
	IsbnList []string
}

//phone,nickname,sex,pro_photo,signature
func CommendClassifyBook(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&CommendClassifyBookObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
	phone := request.Phone
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        fmt.Println(err)
        return
    }
	fmt.Println("连接成功")
	respond := &CommendClassifyBookRespond{}
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
		rows,exit_err := db.Query("select stu_id from user_info where phone=\""+phone+"\"")
		if exit_err != nil {
			respond.Code = Code.UnConfirmStu
			respond.Message = Message.UnConfirmStu
		} else {
			var stu_id string
			for rows.Next() {
				rows.Scan(&stu_id)
			}
			query_err := ClassifyCommend(db,stu_id,respond)
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
		fmt.Println(json_err)
		return
	}
	w.Write(json_respond)

}

func ClassifyHaveBorrow(stu_id string,re *CommendClassifyBookRespond) {
	db,_:=sql.Open(SqlDriver,SqlSourceName)
	defer db.Close()
	//println("尝试连接数据库......")
	if err:=db.Ping();err!=nil {
		fmt.Println(err)
		return
	}
	//println("数据库连接成功!")
	rows,query_err := db.Query(`select isbn,publish_date from book_borrow_history,book_info where title=book_name and stud_id="`+stu_id)
	if query_err!=nil {
		fmt.Println(query_err)
		return
	}
	for rows.Next() {
		var isbn string
		var publish_date int
		rows.Scan(&isbn,&publish_date)
		if publish_date > ClassifyDeadline {
			continue
		}
		_,ok := ReadBook[isbn]
		if ok==false {
			//fmt.Printf("%+v\n", book_name)
			re.IsbnList = append(re.IsbnList, isbn)
			ReadBook[isbn]=true
		}
		
	}
}

func ClassifyCommend(db *sql.DB,stu_id string,re *CommendClassifyBookRespond) error{
	id := UserId[stu_id]
	for k:= range ReadBook {
		 delete(ReadBook, k) 
	}
	for i:=0;i<len(Farmiliar[id]);i++ {
		stud_obj := Farmiliar[id][i]
		familiar_user := UserName[stud_obj.id]
		ClassifyHaveBorrow(familiar_user,re)
	}
	return nil
}