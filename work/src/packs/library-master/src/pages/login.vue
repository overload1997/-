<template>
  <div class="login">
    <div class="input_1">
      <input class="input_1_1" type="text" placeholder="输入手机号" v-model="phone">
    </div>
    <div class="input_2">
      <input class="input_2_1" type="password" placeholder="输入密码" v-model="password">
    </div>
    <div class="button">
      <input class="button_1" @click="Login" type="submit" value="登录">
    </div>
    <div class="forget_it">
      <a class="register" @click="Register">注册</a>
      <span class="sprit">/</span>
      <a class="forget_password" @click="forgetPassword">忘记密码</a>
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
        phone: '',
        password: ''
      }
    },
    methods: {
      //跳转到注册界面
      Register() {
        Toast({
          message: '正在前往注册',
          duration: 600
        })
        setTimeout(() => {
          this.$router.push({
            path: '/register'
          })
        }, 1000)
      },

      //跳转到忘记密码界面
      forgetPassword() {
        Toast({
          message: '正在前往修改密码',
          duration: 600
        })
        setTimeout(() => {
          this.$router.push({
            path: '/forget_password1'
          })
        }, 1000)
      },

      //登录界面
      Login() {
        //手机号码是否正常判断
        if (!checkPhone(this.phone)) {
          Toast({
            message: '请输入合法手机号码',
            duration: 700
          })
          return;
        }
        if (this.phone == '') {
          Toast({
            message: '请输入手机号',
            duration: 700
          })
          return;
        } else if (this.password == '') {
          Toast({
            message: '请输入密码',
            duration: 700
          })
          return;
        } else if (this.password.length < 6) {
          Toast({
            message: '密码不能低于6位',
            duration: 800
          })
          return;
        }
        //现在加入axios
        else {
          axios.post('http://120.79.207.54:5666/user/login', {
              phone: this.phone,
              password: this.password
            })
            .then(res => {
              console.log("success");
              console.log(res);
              if (res.data.Message == "成功") {
                //存放服务器端返回的数据
                console.log("111" + res.data.Sesson.SessonId) //成功
                window.localStorage.setItem("SessionId", res.data.Sesson.SessonId);
                window.localStorage.setItem("Timestamp", res.data.Sesson.Timestamp);
                //将这些东西放到vuex中
                let userInfo={
                    SessionId : res.data.Sesson.SessonId,
                    Timestamp: res.data.Sesson.Timestamp
                }
                this.$store.commit('changeInfo',userInfo);
                // console.log('login中显示'+this.$store.state.SessionId);  成功
                Toast({
                    message: '登录成功',
                    duration: 600
                })
                setTimeout(() => {
                    this.$router.push({
                    path: '/home_page'
                    })
                }, 1000)
              

              } else if (res.data.Message == "帐号不存在") {
                Toast({
                  message: '账号不存在,请重试~',
                  duration: 600
                })
              } else if (res.data.Message == '登录密码错误') {
                Toast({
                  message: '登录密码错误，请重试~',
                  duration: 600
                })
              }

            })
            //对于一些异常直接抛出
            .catch(err => {
                Toast({
                    message:"似乎和外界失联了，再试试吧~",
                    duration:600
                })
            })

        }

      },
    }
  }

</script>
<style>
  .login {
    position: fixed;
    padding: 0;
    margin: 0;
    width: 100vw;
    height: 100vh;
    overflow: hidden;
    left: 0;
    top: 0;
    background-image: url('../../static/log-in-bg.png');
    background-size: 100% 100%;
    background-repeat: no-repeat;
    /* font-size: 16px; */
    font-size: 1rem;
  }

  :-ms-input-placeholder {
    color: #AEAEAE;
    font-size: .444444rem;
    font-family: NotoSansCJKsc-Regular;
    ;
  }

  ::-webkit-input-placeholder {
    color: #AEAEAE;
    font-size: .388889rem;
    font-family: NotoSansCJKsc-Regular;
  }

  .input_1,
  .input_1_1 {
    position: fixed;
    width: 9.111111rem;
    height: 1.333333rem;
    left: .444444rem;
    top: 5.472222rem;
  }

  .input_1_1 {
    width: 9.111111rem;
    padding-left: .138889rem;
    background: #FFFFFF;
    border-radius: 4px;
    border: 0 #AEAEAE;
    -moz-box-shadow: 0 0 10px #D9E7EA;
    -webkit-box-shadow: 0 0 10px #D9E7EA;
    box-shadow: 0 0 10px #D9E7EA;
  }

  .input_2,
  .input_2_1 {
    position: fixed;
    width: 9.111111rem;
    height: 1.333333rem;
    left: .444444rem;
    top: 7.138889rem;
  }

  .input_2_1 {
    width: 9.111111rem;
    padding-left: .138889rem;
    background: #FFFFFF;
    border-radius: 4px;
    border: 0 #AEAEAE;
    -moz-box-shadow: 0 0 10px #D9E7EA;
    -webkit-box-shadow: 0 0 10px #D9E7EA;
    box-shadow: 0 0 10px #D9E7EA;
  }

  .button,
  .button_1 {
    position: fixed;
    width: 9.111111rem;
    height: 1.333333rem;
    left: 0.5rem;
    top: 8.972222rem;
  }

  .button_1 {
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    border-radius: .222222rem;
    border: 0 white;
    color: white;
    font-size: .5rem;
    -moz-box-shadow: 0 0 .277778rem #7BC8E1;
    -webkit-box-shadow: 0 0 .277778rem #7BC8E1;
    box-shadow: 0 0 .277778rem #7BC8E1;
  }

  .forget_it {
    position: fixed;
    left: 4.944444rem;
    top: 10.611111rem;
    height: .5rem;
    width: 1.333333rem;
  }

  .register {
    position: fixed;
    left: 3.694444rem;
    top: 10.611111rem;
    width: .666667rem;
    height: .5rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .333333rem;
    color: #AEAEAE;
    text-align: center;
  }

  .sprit {
    position: fixed;
    left: 4.583333rem;
    top: 10.611111rem;
    width:.138889rem;
    height: .5rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .333333rem;
    color: #AEAEAE;
    text-align: center;
  }

  .forget_password {
    position: fixed;
    text-align: center;
    left: 4.944444rem;
    top: 10.611111rem;
    height: .5rem;
    width: 1.333333rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .333333rem;
    color: #AEAEAE;
   
  }

</style>
