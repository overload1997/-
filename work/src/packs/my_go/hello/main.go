package main

import (
	"fmt"
	"database/sql"
	_ "github.com/Go-SQL-Driver/MySQL"
)

const (
	SqlDriver = "mysql"
	SqlSourceName = "root:123456789+0@tcp(localhost:mysql)/book_app"
	OverdueSeconds = 60*60 
)

func GetTopk(book_type string,k int) error {
	db, db_open_err := sql.Open(SqlDriver, SqlSourceName)
	defer db.Close()
	if db_open_err != nil {
		fmt.Println(db_open_err)
		return db_open_err
	}
	ping_err:=db.Ping()
	if ping_err != nil {
		fmt.Println(ping_err)
		return ping_err
	}
	query_str :="select book_name,count(book_name) from book_borrow_history,book_info where book_name=title and type = \""+book_type+"\" group by book_name  order by count(book_name) desc limit 0,10;"
	//fmt.Printf("%+v\n", query_str)
	rows,db_err:=db.Query(query_str)
	if db_err != nil {
		fmt.Println(db_open_err)
		return db_err
	}
	for rows.Next() {
		var book_name string
		var book_count int
		rows.Scan(&book_name,&book_count)
		fmt.Printf("%s\t%d本\n",book_name,book_count)
	}
	fmt.Printf("\n")
	return nil
}

func main() {
	var dict string="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for i:=0;i<26;i++ {
		var book_type=dict[i:i+1]
		fmt.Printf("类别%s借阅最多的书籍有:\n", book_type)
		GetTopk(book_type,10)
	}
}