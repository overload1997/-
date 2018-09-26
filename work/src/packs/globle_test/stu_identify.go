package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"io/ioutil"
	_ "github.com/Go-SQL-Driver/MySQL"
)

type StuIdentifyObj struct {
	Phone string
	School string
	Stu_id string
	Stu_psw string
	Sesson_id string
}

type StuIdentifyRespond struct {
	Code int
	Message string
}

//phone,nickname,sex,pro_photo,signature
func StuIdentify(w http.ResponseWriter, r *http.Request) {
	fmt.Println("收到http请求") //把  body 内容读入字符串 s
	ReceiveClientRequest(w,r)//调用跨域解决函数           
	str, _ := ioutil.ReadAll(r.Body) //把  body 内容读入字符串 s
	if string(str) =="" {
		return
	}
	fmt.Println(string(str))
	request:=&StuIdentifyObj{}
	err:=json.Unmarshal(str,request)
	if err!=nil {
		log.Fatal(err)
	}
	phone := request.Phone
	school := request.School
	sesson_id := request.Sesson_id
	stu_id := request.Stu_id
	stu_psw := request.Stu_psw

    fmt.Println("phone:",phone)
	fmt.Println("sesson_id:",sesson_id)
	fmt.Println("school:",school)
	fmt.Println("stu_id:",stu_id)
	fmt.Println("stu_psw",stu_psw)

	re := &StuIdentifyRespond{Code.VertifyIng,Message.VertifyIng}
	json,json_err := json.Marshal(re)
	if json_err!=nil {
		fmt.Println(json_err)
		return 
	}
	w.Write(json)
}
