package main

import (
	"database/sql"
	"fmt"
	"os"
	"strings"

	_ "github.com/Go-SQL-Driver/MySQL"
)

const (
	username = "root"
	password = "qw578470347"
	port     = "mysql"
	dbname   = "lib_sql"
	conpro   = "tcp"
)

func printerror(err error) {
	if err == nil {
		return
	}
	fmt.Println(err)
	os.Exit(0)
}

func main() {
	sqlpath := strings.Join([]string{username, ":", password, "@", conpro, "(localhost", ":", port, ")/", dbname, "?charset=utf8"}, "")
	db, err := sql.Open("mysql", sqlpath)
	defer db.Close()
	printerror(err)
	println("尝试ping the ip")
	if err := db.Ping(); err != nil {
		printerror(err)
		return
	}
	fmt.Println("连接成功")
	rows, err := db.Query("select stu_id,stu_name,stu_guake from student")
	if err != nil {
		panic(err)
	}
	for rows.Next() {
		var stu_id int
		var stu_name string
		var stu_guake int
		err = rows.Scan(&stu_id, &stu_name, &stu_guake)
		if err != nil {
			panic(err)
		}
		fmt.Println("学号:", stu_id, " 姓名:", stu_name, " 是否挂过科:", stu_guake)
	}

}
