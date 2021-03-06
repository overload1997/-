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

type GetFamiliarUserObj struct {
	Phone string
	Book_type string
	K int
	Sesson_id string
}

type GetFamiliarUserRespond struct {
	Message string
	Code int
	FamiliarUserList []string
}

//phone,nickname,sex,pro_photo,signature
func GetFamiliarUser(w http.ResponseWriter, r *http.Request) {
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	fmt.Println(string(str))
	request:=&GetFamiliarUserObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	book_type := request.Book_type
	k := request.K
	sesson_id := request.Sesson_id

    fmt.Println("book_type:",book_type)
    fmt.Println("k:",k)
    fmt.Println("sesson_id:",sesson_id) 

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
	fmt.Println("连接成功")
	respond := &GetFamiliarUserRespond{}
	if SessonMap[phone] == nil {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	} else {
		rows,exit_err := db.Query("select stu_id from user_info where phone=\""+phone+"\"")
		if exit_err != nil {
			respond.Code = Code.UnConfirmStu
			respond.Message = Message.UnConfirmStu
		} else {
			var stu_id string
			for rows.Next() {
				rows.Scan(&stu_id)
			}
			query_err := FarmiliarUser(db,stu_id,k,respond)
			if query_err != nil {
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

func FarmiliarUser(db *sql.DB,stu_id string,k int,re *GetFamiliarUserRespond) error{
	var output_count int = 0
	id := UserId[stu_id]
	for i:=0;i<len(Farmiliar[id]);i++ {
		stud_obj := Farmiliar[id][i]
		familiar_user := UserName[stud_obj.id]
		output_count++
		rows,db_err := db.Query("select phone from user_info where stu_id=\""+familiar_user+"\";")
		var familiar_user_phone string = ""
		for rows.Next() {
			rows.Scan(&familiar_user_phone)
		}
		if familiar_user_phone != "" {
			re.FamiliarUserList = append(re.FamiliarUserList, familiar_user_phone)
		}
		if db_err != nil {
			return db_err
		}
		if output_count == k {
			break
		}
	}
	return nil
}