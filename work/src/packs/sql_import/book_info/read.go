// package main

// import (
// 	//"fmt"
// 	"strings"
// 	_ "github.com/Go-SQL-Driver/MySQL"
// 	"io"
// 	"bufio"
// 	"os"
// 	//"errors"
// )

// func solve_er (err error) {
// 	if err!=nil {
// 		panic(err)
// 	}
// }

// func str_format (str string) (error,string) {
// 	return nil,str
// }

// func main() {
// 	fp,fp_err := os.Open("/overload/downloads/book/book")
// 	solve_er(fp_err)
// 	fo,fo_err := os.Create("/overload/downloads/book/book_info")
// 	solve_er(fo_err)
// 	defer fp.Close()
// 	defer fo.Close()
// 	br := bufio.NewReader(fp)
// 	w := bufio.NewWriter(fo)
// 	for {
// 		line,_,end_flag := br.ReadLine()
// 		if end_flag == io.EOF {
// 			break
// 		}
// 		arr:=strings.Split(string(line),";")
// 		if len(arr)<24 {
// 			continue
// 		}
// 		var err3,err4,err5,err6,err7,err10,err11,err23 error
// 		err3,arr[3]=str_format(arr[3])
// 		if err3!=nil {
// 			continue
// 		}
// 		err4,arr[4]=str_format(arr[4])
// 		if err4!=nil {
// 			continue
// 		}
// 		err5,arr[5]=str_format(arr[5])
// 		if err5!=nil {
// 			continue
// 		}
// 		err6,arr[6]=str_format(arr[6])
// 		if err6!=nil {
// 			continue
// 		}
// 		err7,arr[7]=str_format(arr[7])
// 		if err7!=nil {
// 			continue
// 		}
// 		err10,arr[10]=str_format(arr[10])
// 		if err10!=nil {
// 			continue
// 		}
// 		err23,arr[23]=str_format(arr[23])
// 		if err23!=nil {
// 			continue
// 		}
// 		err11,arr[11]=str_format(arr[11])
// 		if err11!=nil {
// 			continue
// 		}
// 		str_err,ins_str:=str_format(arr[3]+"\t"+arr[4]+"\t"+arr[5]+"\t"+arr[6]+"\t"+arr[7]+"\t"+arr[10]+"\t"+arr[23]+"\t"+arr[11]+"\n")
// 		if str_err!=nil {
// 			continue
// 		}
// 		fo.Write([]byte(ins_str))
// 		_, werr := w.WriteString(ins_str)
// 		w.Flush()
// 		solve_er(werr)
// 	}
// 	fo.Sync()
		
// }