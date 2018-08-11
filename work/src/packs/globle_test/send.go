package main

import ypclnt "github.com/sdk"
import "net/http"
import "fmt"
import "encoding/json"
import "log"
import "time"

func Send_message (w http.ResponseWriter, r *http.Request) {
	re := &Respond{}
	phone := r.FormValue("phone")
	fmt.Println("sent indentify code to "+phone)
	clnt := ypclnt.New("87e94501913ea1c29076964f9f8b4c63")
	param := ypclnt.NewParam(2)
	idt_code := Get_rand_num(4)
	param[ypclnt.MOBILE] = phone
	param[ypclnt.TEXT] = "【云片网】您的验证码是" + idt_code
	send_back := clnt.Sms().SingleSend(param)
	back_string:=fmt.Sprintf("%s",send_back)
	if back_string[0]!='0' {
		re.Code=Code.IdtCodeErr
		re.Message=Message.IdtCodeErr
	} else {
		re.Code=Code.Success
		re.Message=Message.Success
		IdentifyCodeMap[phone]=&IdentifyCode{idt_code,time.Now().Unix()}
	}
	re_json,json_err:=json.Marshal(re)
	if json_err!=nil {
		log.Fatal(json_err)
	}
	w.Write(re_json)
}
