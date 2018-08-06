package main

import (
	"log"
	"net/http"
	"packs/book_app/lib_login"
	"packs/book_app/lib_register"
)

const ()

func main() {
	http.HandleFunc("/user/login", lib_login.Login)
	http.HandleFunc("/user/register", lib_register.Register)
	err := http.ListenAndServe(":1521", nil)
	if err != nil {
		log.Fatal(err)
		return
	}
}
