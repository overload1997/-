package main

import (
	"log"
	"net/http"
	"packs/book_app/lib_login"
	"packs/book_app/lib_register"
	"packs/book_app/send_message"
)

const ()

func main() {
	http.HandleFunc("/user/login", lib_login.Login)
	http.HandleFunc("/user/register", lib_register.Register)
	http.HandleFunc("/user/identify_code", send_message.Send_message)
	err := http.ListenAndServe(":1521", nil)
	if err != nil {
		log.Fatal(err)
		return
	}
}
