package main

import (
	"log"
	"net/http"
	"packs/lib_login"
	"packs/lib_register"
)

const ()

func main() {
	http.HandleFunc("/api/login", lib_login.Login)
	http.HandleFunc("/api/register", lib_register.Register)
	err := http.ListenAndServe("localhost:8080", nil)
	if err != nil {
		log.Fatal(err)
		return
	}
}
