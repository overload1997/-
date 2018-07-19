package main

import (
	"log"
	"github.com/PuerkitoBio/goquery"
)

func f(str string) {

	doc, err := goquery.NewDocument("https://read.douban.com" + str)
	if err != nil {
		log.Fatal(err)
	}
	s := doc.Find(".rating .score").Text()
	t := doc.Find(".article-title").Text()
	log.Println("title: " + t + " /score: " + s)
}

func main() {
	doc, err := goquery.NewDocument("https://read.douban.com")
	if err != nil {
		log.Fatal(err)
	}

	doc.Find(".carousel .item").Each(func(i int, contentSelection *goquery.Selection) {
		title, err := contentSelection.Find(".pic").Attr("href")
		if err != true {
			log.Fatal(err)
		}
		f(title)

	})
}
