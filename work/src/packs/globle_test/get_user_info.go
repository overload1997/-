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

type GetUserInfoObj struct {
	Phone string
	Sesson_id int
}

type UserInfo struct {
	Id int
	Password string
	Pro_photo string
	Signature string
	School string
	Student_id string
	Authentication string
	Phone string
	Sex string
	Nickname string
}

type GetUserInfoRespond struct {
	Code int
	Message string
	User UserInfo
}

//phone,nickname,sex,pro_photo,signature
func GetUserInfo(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&StuUpdateObj{}
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
	respond := &GetUserInfoRespond{}
	if SessonMap[phone].SessonId == "" {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	}  else {
		rows,db_err:=db.Query("select * from user_info where phone="+phone)
		if db_err != nil {
			respond.Code = Code.DatabaseErr
			respond.Message = Message.DatabaseErr
		} else {
			respond.Code = Code.Success
			respond.Message = Message.Success	
			for rows.Next() {
				rows.Scan(
					&respond.User.Id,
					&respond.User.Password,
					&respond.User.Pro_photo,
					&respond.User.Signature,
					&respond.User.School,
					&respond.User.Student_id,
					&respond.User.Authentication,
					&respond.User.Phone,
					&respond.User.Sex,
					&respond.User.Nickname,
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
