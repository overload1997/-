package send_message

import ypclnt "github.com/sdk"
import "math/rand"
import "strconv"
import "net/http"
import "fmt"
import "encoding/json"
import "log"
import "time"

func get_rand_num() string {
	rand.Seed(time.Now().UnixNano())
	number := rand.Intn(8999)+1000
	return strconv.Itoa(number)
}

type Respond struct {
	Code int
	Message string
}

func Send_message (w http.ResponseWriter, r *http.Request) {
	re := &Respond{}
	phone := r.FormValue("phone")
	fmt.Println("sent indentify code to "+phone)
	clnt := ypclnt.New("87e94501913ea1c29076964f9f8b4c63")
	param := ypclnt.NewParam(2)
	param[ypclnt.MOBILE] = phone
	param[ypclnt.TEXT] = "【云片网】您的验证码是" + get_rand_num()
	send_back := clnt.Sms().SingleSend(param)
	back_string:=fmt.Sprintf("%s",send_back)
	if back_string[0]!='0' {
		re.Code=20
		re.Message="invalid phone number or system crash."
	} else {
		re.Code=0
		re.Message="success"
	}
	re_json,json_err:=json.Marshal(re)
	if json_err!=nil {
		log.Fatal(json_err)
	}
	w.Write(re_json)
}