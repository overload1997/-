package main

import (
	"strconv"
	"strings"
	"database/sql"
	_ "github.com/Go-SQL-Driver/MySQL"
	"io"
	"bufio"
	"os"
	"fmt"
	"time"
	"sync"
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

func time_format(str string) string {
	return str+" 00:00:00"
}

var index int = 0
var success_insert int = 0

func main() {
	fp,fp_err := os.Open("test.csv")
	solve_err(fp_err)
	defer fp.Close()
	br := bufio.NewReader(fp)
	wch := make(chan struct{},10)
	ins_str:="insert into book_borrow_history (stu_id,book_name,start_time,end_time) values"
	for {
		index++
		line,_,end_flag := br.ReadLine()
		if end_flag == io.EOF {
			break
		}
		if index == 1 {
			continue
		}
		arr:=strings.Split(string(line),"\t")
		start_time_,parse_err1:=time.Parse("2006-1-02 15:04:05",time_format(arr[6]))
		solve_err(parse_err1)
		end_time_,parse_err2:=time.Parse("2006-1-02 15:04:05",time_format(arr[7]))
		solve_err(parse_err2)
		//fmt.Printf("%s %s %d %d\n",arr[1],arr[5],start_time_.Unix(),end_time_.Unix())
		start_time:=strconv.Itoa(int(start_time_.Unix()))
		end_time:=strconv.Itoa(int(end_time_.Unix()))
		ins_str+="(\""+arr[1]+"\",\""+arr[5]+"\","+start_time+","+end_time+"),";
		if index % 10000 != 0 {
			continue
		}
		ins_str=ins_str[:len(ins_str)-1]+";"
		fmt.Printf("%d: %s\n",success_insert+1, ins_str)
		waitgroup.Add(1)
		wch <- struct{}{}
		go func (is string){
			for SqlInsert(is) !=nil{
				fmt.Printf("重新执行 %s\n",is)
			}
			<- wch
			waitgroup.Done()
		}(ins_str)
		ins_str="insert into book_borrow_history (stu_id,book_name,start_time,end_time) values"
		success_insert++
	}
	if ins_str!="insert into book_borrow_history (stu_id,book_name,start_time,end_time) values" {
		ins_str=ins_str[:len(ins_str)-1]+";"
		fmt.Printf("%d: %s\n",success_insert+1, ins_str)
		waitgroup.Add(1)
		wch <- struct{}{}
		go func (is string){
			for SqlInsert(is) !=nil{
				fmt.Printf("重新执行 %s\n",is)
			}
			<- wch
			waitgroup.Done()
		}(ins_str)
	}
	waitgroup.Wait()
	fmt.Printf("%d\n",success_insert)
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
	//solve_err(db_query_err)
	db.Close()
	return nil
}