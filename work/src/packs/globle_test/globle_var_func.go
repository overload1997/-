package main

import "math/rand"
import "time"

var ClassifyDeadline int = 946731696
var RandDigit = []byte{'0','1','2','3','4','5','6','7','8','9'}
var RandChar = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
var SessonMap = make(map[string]*SessonKey)
var IdentifyCodeMap = make(map[string]*IdentifyCode)

const (
	SqlDriver = "mysql"
	SqlSourceName = "root:123456789+0@tcp(localhost:mysql)/book_app"
	OverdueSeconds = 60*60 
)

type IdentifyCode struct {
	IdtCode string
	Timestamp int64
}

type SessonKey struct {
	SessonId string
	Timestamp int64
}

type LoginRespond struct {
	Code    int
	Message string
	Sesson	SessonKey
}

type Respond struct {
	Code    int
	Message string
}

func (obj* SessonKey) Init() {
	obj.GetTimestamp()
	obj.GetSessonKey()
}

func (obj *SessonKey) GetTimestamp() {
	obj.Timestamp=time.Now().Unix()
}

func (obj *SessonKey) GetSessonKey() {
	obj.SessonId=Get_RandString(16)
}

func (obj *SessonKey) add(user string) {
	SessonMap[user]=obj
}

func (obj *SessonKey) CheckOverdue() bool {
	if obj.Timestamp + OverdueSeconds < time.Now().Unix() {
		return true
	}
	return false
}

func (obj *IdentifyCode) CheckOverdue() bool {
	if obj.Timestamp + OverdueSeconds < time.Now().Unix() {
		return true
	}
	return false
}

func Get_rand_num(length int) string {
	rand.Seed(time.Now().Unix())
	rand_str := ""
	for i:=0;i<length;i++ {
		rand_str=rand_str+string(RandDigit[rand.Intn(len(RandDigit))])
	}
	return rand_str
}

func Get_RandString(length int) string {
	rand.Seed(time.Now().Unix())
	rand_str := ""
	for i:=0;i<length;i++ {
		rand_str=rand_str+string(RandChar[rand.Intn(len(RandChar))])
	}
	return rand_str
}


const (
	minpswlen = 8
	maxpswlen = 16
)

func check_password(psw string) bool {
	if len(psw)<minpswlen||len(psw)>maxpswlen {
		return false
	}
	return true 
}