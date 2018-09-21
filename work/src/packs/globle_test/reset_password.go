package main
//phone,password,new_password,sesson_id
import (
	"database/sql"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"io/ioutil"
)

type ResetPasswordObj struct {
	Phone string
	Password string
	New_password string
	Sesson_id string
}

func ResetPassword(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&ResetPasswordObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	password := request.Password
	new_password := request.New_password
	sesson_id := request.Sesson_id
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
		if SessonMap[phone] == nil {
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