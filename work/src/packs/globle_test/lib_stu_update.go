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

type StuUpdateObj struct {
	Phone string
	Nickname string
	Sex string
	Pro_photo string
	Signature string
	Sesson_id string
}
//phone,nickname,sex,pro_photo,signature
func Stu_update(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&StuUpdateObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
    phone := request.Phone
    nickname := request.Nickname
    sex := request.Sex
    pro_photo := request.Pro_photo
	signature := request.Signature
	sesson_id := request.Sesson_id

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
	if SessonMap[phone] == nil {
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
