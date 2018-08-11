//phone,indentify_code,new_password

package main
import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

func ForgetPassword(w http.ResponseWriter, r *http.Request) {
	phone := r.FormValue("phone")
	identify_code := r.FormValue("identify_code")
	new_password := r.FormValue("new_password")
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