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

type SearchBookObj struct {
	Phone string
	Keyword string
	Sesson_id string
}

type SearchBookRespond struct {
	Code int
	Message string
	IsbnList []string
}


//phone,nickname,sex,pro_photo,signature
func SearchBook(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&SearchBookObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
	phone := request.Phone 
	keyword := request.Keyword
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("isbn:",keyword)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        fmt.Println(err)
        return
    }
    fmt.Println("连接成功")
	respond := &SearchBookRespond{}
    if _,ok:=SessonMap[phone] ; !ok {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if _,ok:=SessonMap[phone]; ok&&sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if  _,ok:=SessonMap[phone]; ok && SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	}  else {
		rows,db_err:=db.Query(`select isbn from book_info where isbn!="" and (title like "%`+keyword+`%" or author like "%`+keyword+`%")`)
		fmt.Printf("%+v\n",`select isbn from book_info where isbn!="" and (title like "%`+keyword+`%" or author like "%`+keyword+`%")`)
		if db_err != nil {
			fmt.Printf("%+v\n", db_err)
			respond.Code = Code.DatabaseErr
			respond.Message = Message.DatabaseErr
		} else {
			db.Query(`insert into user_search_log(phone,keyword) values(`+`"`+phone+`","`+keyword+`")`)
			fmt.Printf("%+v\n", `insert into user_search_log(phone,keyword) values(`+`"`+phone+`","`+keyword+`")`)
			respond.Code = Code.Success
			respond.Message = Message.Success	
			var isbn string
			for rows.Next() {
				rows.Scan(&isbn)
				respond.IsbnList = append(respond.IsbnList, isbn)
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
