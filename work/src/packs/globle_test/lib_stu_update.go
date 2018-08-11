//为客户端提供一个登录接口
package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"

	_ "github.com/Go-SQL-Driver/MySQL"
)

//phone,nickname,sex,pro_photo,signature
func Stu_update(w http.ResponseWriter, r *http.Request) {
    phone := r.FormValue("phone")
    nickname := r.FormValue("nickname")
    sex := r.FormValue("sex")
    pro_photo := r.FormValue("pro_photo")
	signature := r.FormValue("signature")
	sesson_id := r.FormValue("sesson_id")

    fmt.Println("phone:",phone)
    fmt.Println("nickname:",nickname)
    fmt.Println("sex:",sex)
    fmt.Println("pro_photo:",pro_photo)
    fmt.Println("signature:",signature)
	fmt.Println("sesson_id:",sesson_id)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &Respond{}
	if SessonMap[phone].SessonId == "" {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	} else if sex!="0" && sex!="1" {
		respond.Code = Code.UpdateDataErr
		respond.Message = Message.UpdateDataErr
	} else {
		_,db_err:=db.Query("update user_info set nickname="+nickname+",sex="+sex+",pro_photo="+pro_photo+",signature="+signature+" where phone="+phone)
		if db_err != nil {
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
