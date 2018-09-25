//phone,indentify_code,new_password

package main
import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"io/ioutil"
)

type ForgetPasswordObj struct {
	Phone string
	Identify_code string
	New_password string
}

func ForgetPassword(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&ForgetPasswordObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	identify_code := request.Identify_code
	new_password := request.New_password
	fmt.Println("phone:",phone)
	fmt.Println("identify_code:",identify_code)
	fmt.Println("new_password:",new_password)
	db, _ := sql.Open(SqlDriver, SqlSourceName)
	defer db.Close()
	println("尝试ping the 数据库")
	respond:=&Respond{}
	if err := db.Ping(); err != nil {
		log.Fatal(err)
		return
	}
	fmt.Println("连接成功")
	rows,_ := db.Query("select phone from user_info where phone="+phone)
	var exit_flag = false
	for rows.Next() {
		exit_flag = !exit_flag
	}
	if !exit_flag {
		respond.Code = Code.AccNone
		respond.Message = Message.AccNone
	} else if check_password(new_password)==false {
		respond.Code = Code.PswFormatErr
		respond.Message = Message.PswFormatErr
	} else {
		if identify_code!=IdentifyCodeMap[phone].IdtCode {
			respond.Code = Code.IdtCodeErr
			respond.Message = Message.IdtCodeErr
		} else if IdentifyCodeMap[phone].CheckOverdue() {
			respond.Code = Code.IdtCodeOverdue
			respond.Message = Message.IdtCodeOverdue
		} else {
			_,db_err := db.Query("update user_info set password="+new_password+" where phone="+phone)
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
		log.Fatal(json_err)
		return
	}
	w.Write(json_respond)
}