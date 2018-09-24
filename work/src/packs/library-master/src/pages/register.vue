<template>
  <div class="register">
    <div class="head">
      <i class="icon iconfont back" @click="Back">&#xe600;</i>
      <div class="border"></div>
      <span class="register_word">注册</span>
    </div>
    <div class="input_content">
      <div class="i1">
        <input class="i1" type="text" placeholder="输入学校" v-model="school">
      </div>
      <div class="i2">
        <input class="i2" type="text" placeholder="输入手机号" v-model="phone">
      </div>
      <div class="i3">
        <input class="i3_3" type="text" placeholder="输入验证码" v-model="identify_code">
        <span v-show="send_AuthCode" class="get"  @click="GetIdentityCode">获取验证码</span>
        <span v-show="!send_AuthCode" class="get"> <span class="get">{{auth_time}}秒后重新发送 </span></span>
      </div>
      <div class="i4">
        <input class="i4" type="password" placeholder="输入密码" v-model="password">
      </div>
      <div class="i5">
        <input class="i5" type="password" placeholder="再次输入密码" v-model="sure_password">
      </div>
      <div class="button">
        <input class="button" type="submit" value="注册" @click="Register">
      </div>
    </div>
  </div>
</template>

<script>
  import {Toast} from 'mint-ui'
  import {checkPhone} from '../js/myBase.js'
  import axios from 'axios'
  export default {
    data() {
      return {
        school: '',
        phone: '',
        identify_code: '',
        password: '',
        sure_password: '',
        auth_time:0 , //倒计时
        send_AuthCode: true //布尔值，通过v-show控制显示‘获取按钮’还是倒计时
      }
    },
    methods: {
      //返回按钮
      Back() {
        this.$router.push({
          path: '/'
        })
      },
      //发送验证码计时器以及axios
      GetIdentityCode() {  
         //手机号码是否正常判断
        if (!checkPhone(this.phone)) {
          Toast({
            message: '请输入合法手机号码',
            duration: 700
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
        .catch(err=>{
          Toast({
            message:"似乎和外界失联了，再试试吧~",
            duration:600
          })
        })
      },
      //注册按钮
      Register() {
        if (this.school == '') {
          Toast({
            message: '请输入学校',
            duration: 700
          })
          return;
        } else if (this.identify_code == '') {
          Toast({
            message: '请输入验证码',
            duration: 700
          })
          return;
        } else if (this.password == '') {
          Toast({
            message: '请输入密码',
            duration: 800
          })
          return;
        } else if (this.sure_password == '') {
          Toast({
            message: '请再次输入密码',
            duration: 800
          })
          return;
        }else if (this.password.length < 6 || this.sure_password.length < 6) {
          Toast({
            message: '密码不得少于6位',
            duration: 800
          })
          return;
        } else if (this.password != this.sure_password) {
          Toast({
            message: '两次密码不一致，请重新输入~',
            duration: 800
          })
          return;
        } else {
          //注册成功跳转到登录界面 使用axios发送请求
          axios.post('http://120.79.207.54:5666/user/register',{
            phone:this.phone,
            identify_code:this.identify_code,
            password:this.password
          })
          .then(res=>{
            console.log(res);
            if(res.data.Message=='success'){
              //注册成功
               Toast({
                message: '注册成功',
                duration: 600
              })
              setTimeout(() => {
                this.$router.push({
                path: '/'
              })
              }, 1000)
            }
            //注册失败————已经注册过
            else if(res.data.Message=="该手机号已经注册过"){
              Toast({
                message:'该手机号已经注册过',
                duration:600
              })
            }
            //注册失败—————验证码错误
            else if(res.data.Message=="验证码错误"){
              Toast({
                message:'验证码错误，请重试',
                duration:600
              })
            }
          })
          //请求失败的时候----连请求的没过去
          .catch(err=>{
            Toast({
              message:"似乎和外界失联了，再试试吧~",
              duration:600
            })
          })
        }
      }
    }
  }

</script>

<style>
  @font-face {
    font-family: "iconfont";
    src: url('../../static/icon2-1/iconfont.eot');
    /* IE9*/
    src: url('../../static/icon2-1/iconfont.eot#iefix') format('embedded-opentype'),
      /* IE6-IE8 */
      url('../../static/icon2-1/iconfont.woff') format('woff'),
      /* chrome, firefox */
      url('../../static/icon2-1/iconfont.ttf') format('truetype'),
      /* chrome, firefox, opera, Safari, Android, iOS 4.2+*/
      url('../../static/icon2-1/iconfont.svg#iconfont') format('svg');
    /* iOS 4.1- */
  }

  .register {
    position: fixed;
    height: 100vh;
    width: 100vw;
  }
  .iconfont {
    font-family: "iconfont" !important;
    font-style: normal;
    -webkit-font-smoothing: antialiased;
    -webkit-text-stroke-width: .005556rem;
    -moz-osx-font-smoothing: grayscale;
  }

  :-ms-input-placeholder {
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #AEAEAE;
  }

  ::-webkit-input-placeholder {
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #AEAEAE;
  }

  /* .head {
    position: fixed;
    height: 3.5rem;
    width: 22.5rem;
  } */

  .back {
    position: fixed;
    line-height: .555556rem;
    top: .5rem;
    left: .5rem;
    font-size: .666667rem;
    color: white;
  }

  .border {
    position: fixed;
    height: 1.555556rem;
    width: 100%;
    z-index: -1;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    -moz-box-shadow: 0 0 .555556rem #c0c2c4;
    -webkit-box-shadow: 0 0 .555556rem #c0c2c4;
    box-shadow: 0 0 .555556rem #c0c2c4;
  }

  .register_word {
    position: fixed;
    top: .444444rem;
    left: 1.722222rem;
    width: 7.611111rem;
    height: .805556rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .555556rem;
    color: #FFFFFF;
    letter-spacing: 0;
  }

  .input_content {
    position: fixed;
    width: 22.5rem;
    top: 7.25rem;
    height: 32.75rem;
  }

  .i1,
  .i2,
  .i3_3,
  .i4,
  .i5 {
    position: fixed;
    left: .444444rem;
    width: 9.111111rem;
    height: 1.333333rem;
    background: #FFFFFF;
    border-radius: .111111rem;
    border: 0 #AEAEAE;
    padding-left: .166667rem;
    -moz-box-shadow: 0 0 .166667rem #AEAEAE;
    -webkit-box-shadow: 0 0 .166667rem #AEAEAE;
    box-shadow: 0 0 .166667rem #AEAEAE;
  }

  .i1 {
    top: 3.222222rem;
  }

  .i2 {
    top: 4.833333rem;
  }

  .i3 {
    position: fixed;
    width: 4.722222rem;
    top: 6.444444rem;
  }

  .i3_3 {
    width: 4.166667rem;
    top: 6.444444rem;
  }

  .get,
  .button
   {
    text-align: center;
    line-height: 1.333333rem;
    position: fixed;
    background: #FFFFFF;
    border-radius: .111111rem;
    border: 0 #AEAEAE;  
    -moz-box-shadow: 0 0 .166667rem #AEAEAE;
    -webkit-box-shadow: 0 0 .166667rem #AEAEAE;
    box-shadow: 0 0 .166667rem #AEAEAE;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    border-radius: .166667rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .388889rem;
    color: #FFFFFF;
  }

  .get {
    left: 5.388889rem;
    height: 1.333333rem;
    width: 4.166667rem;
    top: 6.444444rem;
  }

  .i4 {
    top: 8.055556rem;
  }

  .i5 {
    top: 9.722222rem;
  }

  .button {
    font-size: .5rem;
    top: 11.5rem;
    left: .5rem;
    width: 9.111111rem;
    height: 1.333333rem;
  }

</style>
