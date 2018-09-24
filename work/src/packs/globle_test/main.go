package main

import (
	"log"
	"net/http"
	//"packs/book_app/lib_login"
	//"packs/book_app/lib_register"
	//"packs/book_app/send_message"
	//"packs/book_app/lib_stu_update"
)

var Globle_total_people = 0

func main() {
	CodeInit()
	MessageInit()
	//http.HandleFunc("/", ReceiveClientRequest) //解决跨域问题
	http.HandleFunc("/user/login", Login)
	http.HandleFunc("/user/register", Register)
	http.HandleFunc("/user/identify_code", Send_message)
	http.HandleFunc("/user/stu_update", Stu_update)
	http.HandleFunc("/user/get_fri_list", GetUserFocus)
	http.HandleFunc("/user/add_friend", AddUserFocus)
	http.Handle("/",http.FileServer(http.Dir("./")))
	err := http.ListenAndServe(":5666", nil)
	if err != nil {
		log.Fatal(err)
		return
	}
}