//phone,identify_code,password

package main
import (
	"database/sql"
	"encoding/json"
	"fmt"
	"net/http"
	"io/ioutil"
)

type RegisterObj struct {
	Phone string
	Password string
	Identify_code string
}

func Register(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&RegisterObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		fmt.Println(err)
		return
	}
	phone := request.Phone
	identify_code := request.Identify_code
	password := request.Password
	fmt.Println("phone:",phone)
	fmt.Println("identify_code:",identify_code)
	fmt.Println("password:",password)
	db, _ := sql.Open(SqlDriver, SqlSourceName)
	defer db.Close()
	println("尝试ping the 数据库")
	respond:=&Respond{}
	if err := db.Ping(); err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println("连接成功")
	rows,_ := db.Query("select phone from user_info where phone="+phone)
	var exit_flag = false
	for rows.Next() {
		exit_flag = !exit_flag
	}
	if exit_flag {
		respond.Code = Code.AccExit
		respond.Message = Message.AccExit
	} else if check_password(password)==false {
		respond.Code = Code.PswFormatErr
		respond.Message = Message.PswFormatErr
	} else {
		if _,ok:=IdentifyCodeMap[phone] ; ok==false || identify_code!=IdentifyCodeMap[phone].IdtCode {
			respond.Code = Code.IdtCodeErr
			respond.Message = Message.IdtCodeErr
		} else if _,ok:=IdentifyCodeMap[phone] ; ok && IdentifyCodeMap[phone].CheckOverdue() {
			respond.Code = Code.IdtCodeOverdue
			respond.Message = Message.IdtCodeOverdue
		} else {
			_,db_err := db.Query("insert into user_info(phone,password) values("+phone+","+password+")")
			if db_err != nil {
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