package main

import (
	//"strconv"
	//"strings"
	"database/sql"
	_ "github.com/Go-SQL-Driver/MySQL"
	"io"
	"bufio"
	"os"
	"fmt"
	//"time"
	"sync"
	"bytes"
	"log"
)

var waitgroup sync.WaitGroup

const (
	SqlDriver = "mysql"
	SqlSourceName = "root:123456789+0@tcp(localhost:mysql)/book_app"
	OverdueSeconds = 60*60 
)

func solve_err (err error) {
	if err!=nil {
		panic(err)
	}
}

func EncodeString(s string) string {
	var buf bytes.Buffer
	buf.WriteRune('\'')
	for i := 0 ; i < len(s) ; i++ {
		switch s[i] {
		case 0:
			buf.WriteString(`\0`)
		case '\'':
			buf.WriteString(`\'`)
		case '"':
			buf.WriteString(`\"`)
		case '\b':
			buf.WriteString(`\b`)
		case '\n':
			buf.WriteString(`\n`)
		case '\r':
			buf.WriteString(`\r`)
		case '\t':
			buf.WriteString(`\t`)
		case 26:
			buf.WriteString(`\Z`)
		case '\\':
			buf.WriteString(`\\`)
		default:
			buf.WriteByte(s[i])
		}
	}
	buf.WriteRune('\'')
	return buf.String()
}

func main() {
	for i:=1;i<=2542;i++{
		fmt.Printf("insert %d sql file...\n", i)
		fp,fp_err := os.Open("/overload/downloads/mysql/"+fmt.Sprintf("%d",i)+".csv")
		solve_err(fp_err)
		defer fp.Close()
		br := bufio.NewReader(fp)
		for {
			line,_,end_flag := br.ReadLine()
			if end_flag == io.EOF {
				break
			}
			ins_err:=SqlInsert(string(line))
			if ins_err!=nil {
				log.Fatal(ins_err)
			}
		}
	}
}

func SqlInsert(exec_str string) error{
	db,db_open_err:=sql.Open(SqlDriver,SqlSourceName)
	if db_open_err!=nil {
		db.Close()
		return db_open_err
	}
	//solve_err(db_open_err)
	db_ping_err := db.Ping()
	if db_ping_err!=nil {
		db.Close()
		return db_ping_err
	}
	//solve_err(db_ping_err)
	_,db_query_err:=db.Exec(exec_str)
	if db_query_err!=nil {
		db.Close()
		return db_query_err
	}
	return nil
}

