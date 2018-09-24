<template>
  <div class="forget_password">
    <div class="head">
            <i class="icon iconfont back" @click="Back">&#xe600;</i>
            <div class="top"></div>
            <span class="word1">忘记密码</span>
    </div>
    <div class="inner">
        <input type="text" placeholder="输入手机号" class="phone_number" v-model="phone">
        <div class="test">
            <input type="text" placeholder="输入验证码" class="input_number" v-model="identify_code">
            <!-- <input type="button" value="获取验证码" class="get" @click="GetIdentityCode"> -->
            <span v-show="send_AuthCode" class="get"  @click="GetIdentityCode">获取验证码</span>
            <span v-show="!send_AuthCode" class="get"> <span class="get">{{auth_time}}秒后重新发送</span></span>
        </div>
        <input type="button" value="下一步" class="confirm" @click="Next">
    </div>
</div>
</template>

<script>
import forget_password2 from '@/pages/forget_password2'
import login from '@/pages/login'
import axios from 'axios'
import {Toast} from 'mint-ui'
import {checkPhone} from '../js/myBase.js'  //对输入的手机号码进行中正则
export default {
    data() {
      return {
        phone: '', 
        identify_code:'',  //验证码
        auth_time:0, //倒计时
        send_AuthCode:true //布尔值，通过v-show控制显示‘获取按钮’还是倒计时
      }
    },
  methods:{
    //回退到登录界面
    Back(){
      this.$router.push({path:'/'})
    },
    GetIdentityCode() {  
        if(!checkPhone(this.phone)){
            Toast({
                message:'请输入合法的手机号码',
                duration:600
            })
            return;
        }
        //axios发送验证码
        axios.post('http://120.79.207.54:5666/user/identify_code',{
          phone:this.phone
        })
        .then(res=>{
          Toast({
            message:"验证码已发送，注意查收~",
            duration:600
          });
        //把验证码放到vuex中
        let info={
          phone:this.phone,
          identify_code:this.identify_code
        };
        this.$store.commit('saveIdentityCodeAndPhone',info);
        
        //验证码的倒计时操作
        this.send_AuthCode = false;
        this.auth_time = 30;
        var auth_timetimer =  setInterval(()=>{
          this.auth_time--;
          if(this.auth_time<=0){
            this.send_AuthCode = true;
            clearInterval(auth_timetimer);
          }
        }, 1000);
        })
        //异常
        .catch(err=>{
          Toast({
            message:"似乎和外界失联了，再试试吧~",
            duration:600
          })
        })
      },
    //进入到下一步
    Next(){
      //这里无法判断，只能进行非空的判断
      if(this.identify_code==''){
          Toast({
              message:'验证码不能为空',
              duration:600
          })
          return;
      }
      this.$router.push({path:'/forget_password2'})
    },
   
  }
}
</script>

<style>
@font-face{
  font-family: "iconfont";
  src: url('../../static/icon2-1/iconfont.eot'); /* IE9*/
  src: url('../../static/icon2-1/iconfont.eot#iefix') format('embedded-opentype'), /* IE6-IE8 */
  url('../../static/icon2-1/iconfont.woff') format('woff'), /* chrome, firefox */
  url('../../static/icon2-1/iconfont.ttf') format('truetype'), /* chrome, firefox, opera, Safari, Android, iOS 4.2+*/
  url('../../static/icon2-1/iconfont.svg#iconfont') format('svg'); /* iOS 4.1- */
}

.iconfont {
  font-family:"iconfont" !important;
  font-style:normal;
  -webkit-font-smoothing: antialiased;
  -webkit-text-stroke-width: .005556rem;
  -moz-osx-font-smoothing: grayscale;
}

.forget_password{
  position: fixed;
  padding: 0;
  margin: 0;
  width: 100vw;
  height: 100vh;
  overflow: hidden;
  left: 0;
  top: 0;

}
:-ms-input-placeholder{
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #AEAEAE;
}

::-webkit-input-placeholder{
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #AEAEAE;
}


.head{
  position: fixed;
  left: 0;
  top:0;
  width: 100vw;
  height: 1.555556rem;
}

.back{
    position: fixed;
    top:.5rem;
    left: .5rem;
    height: .555556rem;
    width:.555556rem;
    z-index: 2;
    color: white;
    font-size: .555556rem;
}

.top{
    position: fixed;
    width: 100%;
    top:0;
    left: 0;
    height: 1.555556rem;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    -moz-box-shadow:0 0 .555556rem #d5d7d8;
    -webkit-box-shadow:0 0 .555556rem #d5d7d8;
    box-shadow:0 0 .555556rem #d5d7d8;
}

.word1{
    position: fixed;
    left: 1.722222rem;
    top:.444444rem;
    width: 7.611111rem;
    height: .805556rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .555556rem;
    color: #FFFFFF;
    letter-spacing: 0;
}

.inner{
  position: absolute;
  top:6.25rem;
  left: 1rem;
  width: 100%;
}

.phone_number{
    position: fixed;
    width: 9.111111rem;
    height: 1.333333rem;
    top:2.055556rem;
    padding-left: .166667rem;
    left: .444444rem;
    background: #FFFFFF;
    border-radius: .111111rem;
    border: 0 white;
    -moz-box-shadow:0 0 .277778rem #c9cccf;
    -webkit-box-shadow:0 0 .277778rem #c9cccf;
    box-shadow:0 0 .277778rem #c9cccf;
}

.input_number{
    position: fixed;
    left: .444444rem;
    top: 3.666667rem;
    width: 4.722222rem;
    height: 1.333333rem;
    padding-left: .166667rem;
    background: #FFFFFF;
    border-radius: .111111rem;
    border: 0 white;
    -moz-box-shadow:0 0 .277778rem #c9cccf;
    -webkit-box-shadow:0 0 .277778rem #c9cccf;
    box-shadow:0 0 .277778rem #c9cccf;
}

.get{
    line-height: 1.333333rem;
    position: fixed;
    width: 4.166667rem;
    height: 1.333333rem;
    top:3.666667rem;
    left: 5.555556rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #FFFFFF;
    text-align: center;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    border-radius: .222222rem;
    border: 0 white;
    -moz-box-shadow:0 0 .277778rem #c9cccf;
    -webkit-box-shadow:0 0 .277778rem #c9cccf;
    box-shadow:0 0 .277778rem #c9cccf;
}

.confirm{
    position: fixed;
    left: .444444rem;
    width: 9.111111rem;
    height: 1.333333rem;
    top:5.333333rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .5rem;
    color: #FFFFFF;
    text-align: center;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    border-radius: .222222rem;
    border: 0 white;
    -moz-box-shadow:0 0 .277778rem #c9cccf;
    -webkit-box-shadow:0 0 .277778rem #c9cccf;
    box-shadow:0 0 .277778rem #c9cccf;
}
</style>
