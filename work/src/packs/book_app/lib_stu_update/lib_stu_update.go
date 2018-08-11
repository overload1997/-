//为客户端提供一个登录接口
package lib_stu_update

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
	Code    int
	Message string
}

//phone,nickname,sex,pro_photo,signature
func Stu_update(w http.ResponseWriter, r *http.Request) {
    phone := r.FormValue("phone")
    nickname := r.FormValue("nickname")
    sex := r.FormValue("sex")
    pro_photo := r.FormValue("pro_photo")
    signature := r.FormValue("signature")
    fmt.Println("phone:",phone)
    fmt.Println("nickname:",nickname)
    fmt.Println("sex:",sex)
    fmt.Println("pro_photo:",pro_photo)
    fmt.Println("signature:",signature)
    db, _ := sql.Open(Sql_driver,Sql_source_name)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
    rows, _ := db.Query("select phone from user_info where phone="+phone)
    userexit := false //数据库中存不存在该账户
	respond := &Respond{}
	for rows.Next() {
		userexit = true
	}
	if !userexit {
		respond.Code = 10
		respond.Message = "请输入正确的帐号"
	} else {
		_,err2:=db.Query("update user_info set nickname="+nickname+",sex="+sex+",pro_photo="+pro_photo+",signature="+signature+" where phone="+phone)
		if err2!=nil {
			respond.Code = 30
			respond.Message = "数据不合法"
		} else {
        	respond.Code = 0
			respond.Message = "修改成功"
		}
    }
	re, err1 := json.Marshal(respond)
	if err1 != nil {
		log.Fatal(err1)
		return
	}
	w.Write(re)

}
