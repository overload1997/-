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
	"bytes"
	//"log"
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

// 3 4 5 6 7 10 11 23
func time_format(str string) string {
	for len(str)<8 {
		str+="0"
	}
	str=str[:4]+"-"+str[4:6]+"-"+str[6:]
	if str[5]=='0' && str[6]=='0' {
		str=str[:5]+"1"+str[6:]
	}
	if str[8]=='0' && str[9]=='0' {
		str=str[:8]+"1"+str[9:]
	}
	return str+" 00:00:00"
}

var index int = 0
var success_insert int = 0

func EncodeString(s string) string {
	if len(s)>1000000 {
		return ""
	}
	var buf bytes.Buffer
	//buf.WriteRune('\'')
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
	//buf.WriteRune('\'')
	return buf.String()
}

func main() {
	fp,fp_err := os.Open("/overload/downloads/book/book_info")
	solve_err(fp_err)
	defer fp.Close()
	br := bufio.NewReader(fp)
	wch := make(chan struct{},10)
	ins_str:="insert into book_info (title,author,publish,publish_address,publish_date,type,summary,isbn) values"
	//fo,fo_err := os.Create("/overload/downloads/mysql/0.csv")
	//w := bufio.NewWriter(fo)
	//solve_err(fo_err)
	ins_index:=0
	file_index:=0
	index:=0
	for {
		index++
		line,_,end_flag := br.ReadLine()
		if end_flag == io.EOF {
			break
		}
		if index%2 ==0 {
			continue
		}
		arr:=strings.Split(string(line),"\t")
		if len(arr)<7 {
			continue
		}
		pub_time:="0"
		if arr[4]!="0" {
			pub_time_,parse_err:=time.Parse("2006-01-02 15:04:05",time_format(arr[4]))
			if parse_err == nil {
				pub_time=strconv.Itoa(int(pub_time_.Unix()))
			} else {
				continue
			}
		}
		/*if len(arr[7])>20 {
			arr[7]=""
		}
		if len(arr[5])>1 {
			arr[5]=""
		}*/
		arr[0]=EncodeString(arr[0])
		arr[1]=EncodeString(arr[1])
		arr[2]=EncodeString(arr[2])
		arr[3]=EncodeString(arr[3])
		arr[4]=EncodeString(arr[4])
		arr[5]=EncodeString(arr[5])
		arr[6]=EncodeString(arr[6])
		arr[7]=EncodeString(arr[7])
		//arr[0]=arr[0][1:len(arr[0])-1]
		var alp_cnt int=0;
		var alp string
		for i:=0;i<len(arr[5]);i++ {
			if 'A'<=arr[5][i]  && arr[5][i]<='Z' {
				alp_cnt++
				alp=arr[5][i:i+1]
			}
			if alp_cnt > 1 {
				break
			}
		}
		if alp_cnt != 1 {
			arr[5]=""
		} else {
			arr[5]=alp
		}
		ins_str=ins_str+"(\""+arr[0]+"\",\""+arr[1]+"\",\""+arr[2]+"\",\""+arr[3]+"\","+pub_time+",\""+arr[5]+"\",\""+arr[6]+"\",\""+arr[7]+"\"),";
		ins_index++		
		if len(ins_str)<=2000000 && ins_index%5000 != 0 {
			continue
		}
		ins_str=ins_str[:len(ins_str)-1]+";"
		file_index++
		fmt.Printf("%d insert ...\n", ins_index)
		//fo,fo_err = os.Create("/overload/downloads/mysql/"+fmt.Sprintf("%d",file_index)+".csv")			
		//solve_err(fo_err)
		//w = bufio.NewWriter(fo)
		waitgroup.Add(1)
		wch <- struct{}{}
		//fo.Write([]byte(ins_str))
		//_, werr := w.WriteString(ins_str)
		//w.Flush()
		//solve_err(werr)
		
		go func (is string){
			ins_err := SqlInsert(is)
			if ins_err !=nil{
				fmt.Printf("%+v\n", ins_err)
				//log.Fatal(is)				
			}
			<- wch
			waitgroup.Done()
		}(ins_str)
		ins_str="insert into book_info (title,author,publish,publish_address,publish_date,type,summary,isbn) values"
	}
	if ins_str!="insert into book_info (title,author,publish,publish_address,publish_date,type,summary,isbn) values" {
		ins_str=ins_str[:len(ins_str)-1]+";"
		//fmt.Printf("%d\n",index)
		waitgroup.Add(1)
		wch <- struct{}{}
		go func (is string){
			ins_err := SqlInsert(is)
			for SqlInsert(is) !=nil{
				fmt.Printf("%+v\n", ins_err)
			}
			<- wch
			waitgroup.Done()
		}(ins_str)
		// fo.Write([]byte(ins_str))
		// _, werr := w.WriteString(ins_str)
		// w.Flush()
		// solve_err(werr)
	}
	waitgroup.Wait()
	//fo.Sync()
}

func SqlInsert(exec_str string) error{
	//time.Sleep(time.Millisecond * 1000)
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

