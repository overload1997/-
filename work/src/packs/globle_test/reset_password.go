//phone,password,new_password,sesson_id

package main
import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)


func ResetPassword(w http.ResponseWriter, r *http.Request) {
	phone := r.FormValue("phone")
	password := r.FormValue("password")
	new_password := r.FormValue("new_password")
	sesson_id := r.FormValue("sesson_id")
	fmt.Println("phone:",phone)
	fmt.Println("password:",password)
	fmt.Println("new_password:",new_password)
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
	var flag_password string
	rows,_ := db.Query("select password from user_info where phone="+phone)
	for rows.Next() {
		rows.Scan(&flag_password)
		if SessonMap[phone].SessonId == "" {
			respond.Code = Code.SidNone			
			respond.Message = Message.SidNone
		} else if sesson_id != SessonMap[phone].SessonId {
			respond.Code = Code.SidErr
			respond.Message = Message.SidErr
		} else if SessonMap[phone].CheckOverdue() {
			respond.Code = Code.SidOverdue
			respond.Message = Message.SidOverdue
		} else if flag_password != password {
			respond.Code = Code.PswErr
			respond.Message = Message.PswErr
		} else if check_password(new_password)==false {
			respond.Code = Code.PswFormatErr
			respond.Message = Message.PswFormatErr
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