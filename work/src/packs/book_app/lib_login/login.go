//为客户端提供一个登录接口
package lib_login

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"

	_ "github.com/Go-SQL-Driver/MySQL"
)

const (
	username = "root"
	password = "123456789+0"
	port     = "mysql"
	dbname   = "lib_sql"
	conpro   = "tcp"
)

type Respond struct {
	code    int
	message string
}

//Login接口
func Login(w http.ResponseWriter, r *http.Request) {
	//获取表单
	phone := r.FormValue("phone")
	//password := overload_aes.Unpackkey(r.FormValue("password"))
	password := r.FormValue("password")
	fmt.Printf("id:%s password:%s\n", phone, password)
	//链接数据库
	//sqlpath := strings.Join([]string{username, ":", password, "@", conpro, "(localhost", ":", port, ")/", dbname, "?charset=utf8"}, "")
	db, err := sql.Open("mysql", "root:123456789+0@tcp(localhost:mysql)/book_app")
	defer db.Close()
	println("尝试ping the 数据库")
	if err := db.Ping(); err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println("连接成功")
	//查询处理
	rows, err := db.Query("select stu_password from user_info where phone=\"" + phone + "\"")
	if err != nil {
		log.Fatal(err)
		return
	}
	userexit := false //数据库中存不存在该账户
	respond := &Respond{}
	userexit = true
	var psw string
	err = rows.Scan(&psw)
	if err != nil {
		log.Fatal(err)
		return
	}
	if psw != password {
		respond.code = 11
		respond.message = "请输入正确的密码!"
		//return
	} else {
		respond.code = 0
		respond.message = "密码正确!"
		//return
	}
	if !userexit {
		respond.code = 10
		respond.message = "请输入正确的帐号"
	}
	re, err1 := json.Marshal(respond)
	if err1 != nil {
		log.Fatal(err1)
		return
	}
	w.Write(re)
}
