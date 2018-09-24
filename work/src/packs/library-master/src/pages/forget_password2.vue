<template>
  <div class="forget_password">
    <div class="head">
            <i class="icon iconfont back" @click="Back">&#xe600;</i>
            <div class="top"></div>
            <span class="word1">忘记密码</span>
    </div>
    <div class="inner">
        <input type="password" placeholder="输入新密码" class="password_one" v-model="new_password">
        <input type="password" placeholder="再次输入新密码" class="password_two" v-model="sure_password">
        <input type="button" value="确认" class="confirm" @click="Confirm">
    </div>
</div>
</template>

<script>
import {Toast} from 'mint-ui'
import forget_password1 from '@/pages/forget_password1'
import login from '@/pages/login'
import axios from 'axios';
export default {
    data(){
        return{
            new_password:'',
            sure_password:'',
        }
    },
  methods:{
    Back(){
      this.$router.push({path:'/forget_password1'})
    },
    Confirm(){
        //首先对密码进行一些基本的操作
        if(this.new_password==''||this.sure_password==''){
            Toast({
                message:'新密码不能为空',
                duration:600
            })
            return;
        }else if(this.new_password.length<6){
            Toast({
                message:'新密码不能低于6位',
                duration:600
            })
            return;
        }else if(this.new_password!=this.sure_password){
            Toast({
                message:'两次密码不一致',
                duration:600
            })
            return;
        }else{
            // console.log(this.$store.state.identify_code); //是成功获取到的
            var send_identify_code=this.$store.state.identify_code;
            var send_phone=this.$store.state.phone;
            // alert(send_identify_code+"   "+send_phone);
            //发送axios请求给服务器 对返回的数据进行判断
            axios.post('http://120.79.207.54:5666/user/forget_password',{
                identify_code:send_identify_code,
                new_password:this.new_password,
                phone:send_phone
            })
            //发送请求成功
            .then(res=>{
                //修改成功
                if(res.data.Message=='success'){
                    Toast({
                        message:'修改密码成功',
                        duration:600
                    })
                    setTimeout(()=>{
                        this.$router.push({
                        path: '/'
                        })
                    },1000)
                }else if(res.data.Message=='验证码错误'){
                    Toast({
                        message:'验证码错误',
                        duration:600
                    })
                    return;
                }
            })
            //异常直接跳出
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

forget_password{
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
  position: absolute;
  left: 0;
  top:0;
  width: 100%;
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


.password_one,.password_two{
    position: fixed;
    left: .444444rem;
    width: 9.111111rem;
    height: 1.333333rem;
    top:2.055556rem;
    padding-left: .166667rem;
    background: #FFFFFF;
    border-radius: .111111rem;
    border: 0 white;
    -moz-box-shadow:0 0 .277778rem #c9cccf;
    -webkit-box-shadow:0 0 .277778rem #c9cccf;
    box-shadow:0 0 .277778rem #c9cccf;
}

.password_two{
    top:3.666667rem;
}

.confirm{
    position: fixed;
    left: .444444rem;
    width: 9.111111rem;
    height: 1.333333rem;
    top:5.5rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: .555556rem;
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
