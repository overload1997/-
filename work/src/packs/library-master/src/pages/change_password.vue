<template>
  <div class="password1">
    <div class="head">
            <i class="icon iconfont back" @click="Back">&#xe600;</i>
            <div class="top"></div>
            <span class="word1">修改密码</span>
    </div>
    <div class="inner">
        <input type="password" placeholder="输入原密码" v-model="password">
        <input type="password" placeholder="输入新密码" class="password_one" v-model="new_password">
        <input type="password" placeholder="再次输入密码" class="password_two" v-model="sure_password">
        <input type="button" value="确认" class="confirm" @click="Confirm">
    </div>
</div>
</template>

<script>
import change_head from '@/pages/change_head'
import {Toast} from 'mint-ui'
import axios from 'axios'
export default {
    data(){
        return{
            password:'',
            new_password:'',
            sure_password:'',
        }
    },
  methods:{
    Back(){
      this.$router.push({path:'/change_head'})
    },
    Confirm(){
        //首先对密码进行非空和是否相同的判断
        if(this.password==''){
            Toast({
                message:'原密码不能为空',
                duration:600
            })
            return;
        }else if(this.new_password==''||this.sure_password==''){
            Toast({
                message:'新密码不能为空',
                duration:600
            })
            return;
        }else if(this.new_password.length<6||this.sure_password.length<6){
            Toast({
                message:'新密码长度不能小于6',
                duration:600
            })
            return;
        }else if(this.new_password!=this.sure_password){
            Toast({
                message:'新密码不一致',
                duration:600
            })
            return;
        }
        else if(this.password==this.new_password){
            Toast({
                message:'新密码不能和旧密码一致',
                duration:600
            })
            return;
        }
        else{
            var send_sesson_id=this.$store.state.SessionId;
            //发送axios请求
            axios.post('http://120.79.207.54:5666/user/reset_password',{
                password:this.password,
                new_password:this.new_password,
                sesson_id:send_sesson_id,
            })
            .then(res=>{
                //修改成功
                if(res.data.Message=='success'){
                    Toast({
                        message:'修改密码成功!',
                        duration:600
                    })
                     setTimeout(()=>{
                        this.$router.push({
                        path: 'change_head'
                    })
                },1000)
                }
            })
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
  -webkit-text-stroke-width: 0.2px;
  -moz-osx-font-smoothing: grayscale;
}

body{
    width: 100%;
    font-size: 16px;
}

:-ms-input-placeholder{
    font-family: NotoSansCJKsc-Regular;
    font-size: 14px;
    color: #AEAEAE;
}

::-webkit-input-placeholder{
    font-family: NotoSansCJKsc-Regular;
    font-size: 14px;
    color: #AEAEAE;
}


.head{
  position: absolute;
  left: 0;
  top:0;
  width: 100%;
}

.back{
    position: absolute;
    top:1.125rem;
    left: 1.125rem;
    height: 1.25rem;
    width:1.25rem;
    z-index: 2;
    color: white;
    font-size: 20px;
}

.top{
    position: absolute;
    width: 100%;
    top:0;
    left: 0;
    height: 3.5rem;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    -moz-box-shadow:0 0 20px #d5d7d8;
    -webkit-box-shadow:0 0 20px #d5d7d8;
    box-shadow:0 0 20px #d5d7d8;
}

.word1{
    position: absolute;
    left: 3.875rem;
    top:.89375rem;
    width: 17.125rem;
    height: 1.8125rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: 1.25rem;
    color: #FFFFFF;
    letter-spacing: 0;
}

.inner{
  position: absolute;
  top:6.25rem;
  left: 1rem;
  width: 100%;
}

.password_one,.password_two{
    position: absolute;
    width: 20rem;
    height: 3rem;
    top:0;
    padding-left: 0.5rem;
    background: #FFFFFF;
    border-radius: 4px;
    border: 0 white;
    -moz-box-shadow:0 0 10px #c9cccf;
    -webkit-box-shadow:0 0 10px #c9cccf;
    box-shadow:0 0 10px #c9cccf;
}

.password_two{
    top:4rem;
}

.confirm{
    position: absolute;
    width: 20.5rem;
    height: 3rem;
    top:9rem;
    font-family: NotoSansCJKsc-Regular;
    font-size: 18px;
    color: #FFFFFF;
    text-align: center;
    background-image: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
    border-radius: 8px;
    border: 0 white;
    -moz-box-shadow:0 0 10px #c9cccf;
    -webkit-box-shadow:0 0 10px #c9cccf;
    box-shadow:0 0 10px #c9cccf;
}
</style>