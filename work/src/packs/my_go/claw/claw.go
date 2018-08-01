package main

import (
	"fmt"
	"log"
	"os"
	"time"

	"github.com/PuerkitoBio/goquery"
)

const (
	maxpages    = 10
	doubanaddr  = "/home/acm/work/claw/douban.txt"
	doubanlink  = "https://read.douban.com"
	yamaxunaddr = "/home/acm/work/claw/yamaxun.txt"
	yamaxunlink = "https://www.amazon.cn"
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

//豆瓣书籍名称+评分
func DoubanDeep1(str string) {

	doc, err := goquery.NewDocument(doubanlink + str)
	if err != nil {
		log.Fatal(err)
	}
	s := doc.Find(".rating .score").Text()
	t := doc.Find(".article-title").Text()
	LOG := "title: " + t + " /score: " + s + "\n"
	log.Println(LOG)
	WriteLog(LOG, doubanaddr)
}

//爬取豆瓣的书籍，主函数
func crawdouban() {
	doc, err := goquery.NewDocument(doubanlink)
	if err != nil {
		log.Fatal(err)
	}

	doc.Find(".carousel .item").Each(func(i int, contentSelection *goquery.Selection) {
		title, err := contentSelection.Find(".pic").Attr("href")
		if err != true {
			log.Fatal(err)
		}
		DoubanDeep1(title)
	})
}

//爬取亚马逊书籍评分
func Yamaxunbookscore(str string) {
	// fmt.Println(str)
	d, err := goquery.NewDocument(str)
	if err != nil {
		log.Fatal(err)
	}
	time.Sleep(time.Second * 2)
	d.Find("#mainResults li").Each(func(i int, contentSelection *goquery.Selection) {
		nextpage := contentSelection.Find("h2").Text()
		score := contentSelection.Find(".a-icon-alt").Text()
		WriteLog("书名：<<"+nextpage+">>------------------------ 评分："+score+"\n", yamaxunaddr)
		fmt.Println("书名：<<"+nextpage+">>------------------------ 评分：", score)
	})

}

//爬取亚马逊书籍网页的下一页
func Yamaxundfs(str string, pages int) {
	str = yamaxunlink + str
	Yamaxunbookscore(str)
	//递归进入下一页
	//if pages <= maxpages {
	// , err := goquery.NewDocument(str)
	// if err != nil {
	// 	log.Fatal(err)
	// }
	//}
}

//爬取亚马逊图书主函数
func crawyamaxun() {
	//进入电子书主页
	doc, err := goquery.NewDocument(yamaxunlink + "/b/?&node=116169071&tag=baiduiclickcn-23&ref=DEP_18119_SY_34")
	if err != nil {
		log.Fatal(err)
	}
	//爬取该页面的第四个模块，即图书分类
	doc.Find(".a-section .a-spacing-base .browseBox ul").Each(func(i int, contentSelection *goquery.Selection) {
		if i == 4 {
			contentSelection.Find("li").Each(func(i int, contentSelection *goquery.Selection) {
				str := contentSelection.Find("a b").Text()
				if str == "" {
					//下一层的地址
					childname := contentSelection.Find("a").Text()
					childlink, _ := contentSelection.Find("a").Attr("href")
					fmt.Println(childname)
					WriteLog(childname+":\n", yamaxunaddr)
					//time.Sleep(time.Second * 2)
					Yamaxundfs(childlink, 1)
				}
			})
		}
	})
}

func main() {
	//crawdouban()
	crawyamaxun()

}
