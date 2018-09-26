//为客户端提供一个登录接口
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

type UserFocusObj struct {
	Phone string
	Sesson_id string
}

type FocurUser struct {
	Phone string
	Nickname string
	Sinature string
	School string
	Pro_photo string
}

type UserFocusRespond struct {
	Code int
	Message string
	Focus_users []FocurUser
}

//phone,nickname,sex,pro_photo,signature
func GetUserFocus(w http.ResponseWriter, r *http.Request) {
	fmt.Println("受到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&UserFocusObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
    phone := request.Phone
	sesson_id := request.Sesson_id

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)

    db, _ := sql.Open(SqlDriver, SqlSourceName)
    defer db.Close()
    println("尝试ping the 数据库")
    if err := db.Ping(); err != nil {
        log.Fatal(err)
        return
    }
    fmt.Println("连接成功")
	respond := &UserFocusRespond{}
    if _,ok:=SessonMap[phone] ; !ok {
		respond.Code = Code.SidNone			
		respond.Message = Message.SidNone
	} else if _,ok:=SessonMap[phone]; ok&&sesson_id != SessonMap[phone].SessonId {
		respond.Code = Code.SidErr
		respond.Message = Message.SidErr
	} else if  _,ok:=SessonMap[phone]; ok && SessonMap[phone].CheckOverdue() {
		respond.Code = Code.SidOverdue
		respond.Message = Message.SidOverdue
	} else {
		rows,db_err:=db.Query("select user_y from user_focus_user where user_x="+phone)
		fmt.Printf("%s\n", "select user_y from user_focus_user where user_x="+phone)
		if db_err != nil {
			fmt.Printf("%s\n", db_err)
			respond.Code = Code.DatabaseErr
			respond.Message = Message.DatabaseErr
		} else {
			respond.Code = Code.Success
			respond.Message = Message.Success	
			var focus_user string
			for rows.Next() {
				rows.Scan(&focus_user)
				user_query := "select phone,nickname,signature,school,pro_photo from user_info where phone=\""+focus_user+"\""
				fmt.Println(user_query)
				user,user_err:=db.Query(user_query)
				if user_err!=nil {
					fmt.Println(user_err)
					return
				}
				for user.Next() {
					focus_user := FocurUser{}
					user.Scan(&focus_user.Phone,&focus_user.Nickname,&focus_user.Sinature,&focus_user.School,&focus_user.Pro_photo)
					respond.Focus_users = append(respond.Focus_users, focus_user)
				}
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
