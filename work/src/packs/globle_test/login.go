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

type LoginObj struct {
	Phone string
	Password string
}

func Login(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&LoginObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	password := request.Password
	fmt.Printf("id:%s password:%s\n", phone, password)
	//链接数据库
	db, _ := sql.Open(SqlDriver, SqlSourceName)
	defer db.Close()
	println("尝试ping the 数据库")
	if err := db.Ping(); err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println("连接成功")
	//查询处理
	rows, err := db.Query("select password from user_info where phone=" + phone)
	if err != nil {
		log.Fatal(err)
		return
	}
	userexit := false //数据库中存不存在该账户
	respond := &LoginRespond{}
	var user_sesson SessonKey
	for rows.Next() {
		userexit = true
		var psw string
		err = rows.Scan(&psw)
		if err != nil {
			log.Fatal(err)
			return
		}
		if psw != password {
			respond.Code = Code.PswErr
			respond.Message = Message.PswErr
		} else {
			respond.Code = Code.Success
			respond.Message = Message.Success
			user_sesson.Init()
			user_sesson.add(phone)
		}
	}
	if !userexit {
		respond.Code = Code.AccNone
		respond.Message = Message.AccNone
	}
	respond.Sesson=user_sesson
	json_respond, json_err := json.Marshal(respond)
	if json_err != nil {
		log.Fatal(json_err)
		return
	}
	w.Write(json_respond)
}
