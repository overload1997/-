//为客户端提供一个登录接口
package lib_login

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"packs/overload_aes"

	_ "github.com/Go-SQL-Driver/MySQL"
)

const (
	username = "root"
	password = "qw578470347"
	port     = "mysql"
	dbname   = "lib_sql"
	conpro   = "tcp"
)

type Respond struct {
	Message string
}

//Login接口
func Login(w http.ResponseWriter, r *http.Request) {
	//获取表单
	user := r.FormValue("user")
	password := overload_aes.Unpackkey(r.FormValue("password"))
	fmt.Printf("user:%s password:%s\n", user, password)
	//链接数据库
	//sqlpath := strings.Join([]string{username, ":", password, "@", conpro, "(localhost", ":", port, ")/", dbname, "?charset=utf8"}, "")
	db, err := sql.Open("mysql", "root:qw578470347@tcp(localhost:mysql)/lib_sql")
	defer db.Close()
	println("尝试ping the 数据库")
	if err := db.Ping(); err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println("连接成功")
	//查询处理
	rows, err := db.Query("select stu_password from student where stu_id=" + user)
	if err != nil {
		log.Fatal(err)
		return
	}
	userexit := false //数据库中存不存在该账户
	respond := &Respond{}
	for rows.Next() {
		userexit = true
		var psw string
		err := rows.Scan(&psw)
		if err != nil {
			log.Fatal(err)
			return
		}
		if psw != password {
			respond.Message = "请输入正确的密码!"
			//return
		} else {
			respond.Message = "密码正确!"
			//return
		}
	}
	if !userexit {
		respond.Message = "请输入正确的帐号"
	}
	re, err1 := json.Marshal(respond)
	if err1 != nil {
		log.Fatal(err1)
		return
	}
	w.Write(re)
}
