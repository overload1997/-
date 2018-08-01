package main

import (
	"fmt"
	"log"
	"os"

	"github.com/PuerkitoBio/goquery"
)

const (
	Doubanbookx = "https://book.douban.com/subject_search?search_text="
	Doubanbooky = "&cat=1001"
	Yamaxunbook = "https://www.amazon.cn/s/ref=nb_sb_noss_2?__mk_zh_CN=亚马逊网站&url=search-alias%3Dstripbooks&field-keywords="
	Ybook       = "https://www.amazon.cn/s/ref=nb_sb_noss?__mk_zh_CN=亚马逊网站&url=node%3D116169071&field-keywords="
)

//记录到本地文件
func WriteLog(Log string, Addr string) {
	f, err := os.OpenFile(Addr, os.O_CREATE|os.O_APPEND|os.O_RDWR, os.ModePerm|os.ModeTemporary)
	if err != nil {
		fmt.Printf("%s\n", err)
	}
	log := []byte(Log)
	f.Write(log)
	defer f.Close()
}

func Getbookinfo(name string) {
	//link := Ybook + name
	//fmt.Printf("地址:%s\n", link)
	doc, err := goquery.NewDocument("https://www.qidian.com/search?kw=生在唐人街")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(doc)
	fmt.Println(doc.Find(".result-list").Text())
	//Yamaxunbookscore(link)
}

func main() {
	//crawdouban()
	//crawyamaxun()
	Getbookinfo("简爱")
}
