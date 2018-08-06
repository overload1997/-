package lib_register

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

const (
	username  = "root"
	password  = "qw578470347"
	port      = "mysql"
	dbname    = "lib_sql"
	conpro    = "tcp"
	minpswlen = 8
	maxpswlen = 16
)

type Respond struct {
	Message string
}

func Register(w http.ResponseWriter, r *http.Request) {
	//获取表单
	user := r.FormValue("user")
	password := r.FormValue("password")
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
	}
	if userexit {
		respond.Message = "您的帐号已存在"
	} else if len(password) < minpswlen || len(password) > maxpswlen {
		respond.Message = "长度应该为" + fmt.Sprintf("%d", minpswlen) + "~" + fmt.Sprintf("%d", maxpswlen) + "位"
		fmt.Println("密码长度出现错误")
	} else {
		quertstr := "insert into student(stu_id ,stu_name,stu_password) values (" + user + ",\"#\",\"" + password + "\")"
		fmt.Println(quertstr)
		_, err := db.Query(quertstr)
		if err != nil {
			fmt.Println(err)
			respond.Message = "内部出错,请联系管理员"
		} else {
			fmt.Println("注册成功")
			respond.Message = "注册成功"
			fmt.Printf("---帐号:%s 密码:%s\n", user, password)
		}
	}
	re, _ := json.Marshal(respond)
	w.Write(re)
}
