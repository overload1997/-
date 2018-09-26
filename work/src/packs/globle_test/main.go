package main

import (
	"log"
	"net/http"
	"fmt"
	//"packs/book_app/lib_login"
	//"packs/book_app/lib_register"
	//"packs/book_app/send_message"
	//"packs/book_app/lib_stu_update"
)

var Globle_total_people = 0

func main() {
	fmt.Println("Server start...")
	CodeInit()
	MessageInit()
	//http.HandleFunc("/", ReceiveClientRequest) //解决跨域问题
	http.HandleFunc("/user/login", Login)
	http.HandleFunc("/user/register", Register)
	http.HandleFunc("/user/identify_code", Send_message)
	http.HandleFunc("/user/update", Stu_update)
	http.HandleFunc("/user/get_fri_list", GetUserFocus)
	http.HandleFunc("/user/add_friend", AddUserFocus)
	http.HandleFunc("/user/get_user_info", GetUserInfo)
	//http.HandleFunc("/user/add_friend", AddUserFocus)
	http.HandleFunc("/globle/get_borrow_topk", GetBorrowTopk)
	http.HandleFunc("/user/get_same_k", GetFamiliarUser)
	http.HandleFunc("/user/get_bor_history",GetUserBorrow )
	http.HandleFunc("/user/get_user_favor", GetUserCollect)
	http.HandleFunc("/user/add_favor_book", AddUserCollect)
	http.HandleFunc("/user/drop_favor_book",DropUserCollect )
	http.HandleFunc("/user/add_book_comment",AddBookComment )
	http.HandleFunc("/globle/query_book_comment", QueryBookComment )
	http.HandleFunc("/user/get_user_comment", QueryUserComment)
	http.HandleFunc("/globle/get_book_info",GetBookInfo )
	http.HandleFunc("/user/reset_password",ResetPassword)
	http.HandleFunc("/user/forget_password",ForgetPassword)
	http.HandleFunc("/user/recommend_book",CommendBook)
	//http.Handle("/",http.FileServer(http.Dir("./")))
	err := http.ListenAndServe(":5666", nil)
	if err != nil {
		log.Fatal(err)
		return
	}
}
