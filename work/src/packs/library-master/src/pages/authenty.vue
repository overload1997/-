<template>
  <div class="authenty">
      <div class="head">
            <i class="icon iconfont back" @click="Back">&#xe600;</i>
            <div class="top"></div>
            <span class="word1">认证</span>
      </div>
      <div class="info">
        <input type="text" class="phone_number" placeholder="请输入手机号" v-model="phone">
        <input type="text" class="id_number" placeholder="请输入学校" v-model="school">
        <input type="text" class="id_password" placeholder="请输入教务系统学号" v-model="stu_id">
        <input type="password" class="this_password" placeholder="请输入教务系统密码" v-model="stu_psw">
      </div>
      <input type="button" class="confirm" value="认证" @click="Confirm">
  </div>
</template>

<script>
import {Toast} from 'mint-ui'
import my from '@/pages/my'
import {checkPhone} from '../js/myBase.js'
import axios from 'axios';
export default {
  data(){
    return{
      phone:'',
      school:'',
      stu_id:'',
      stu_psw:''
    }
  },
  methods:{
    Back(){
      this.$router.push({
        path:'/my'
      })
    },
    Confirm(){
      if(!checkPhone(this.phone)){
        Toast({
          message:'请输入合法手机号',
          duration:1000
        })
        return;
      }else if(this.school!='广州大学'){
        Toast({
          message:'很抱歉，系统暂不支持该学校',
          duration:1000
        })
        return;
      }else if(this.stu_id==''){
        Toast({
          message:'教务系统学号不能为空',
          duration:1000
        })
        return;
      }else if(this.stu_psw==''){
        Toast({
          message:'教务系统密码不能为空',
          duration:1000
        })
        return;
      }else{
        //发送axios请求
        var session_id=this.$store.state.sesson_id;
        axios.post('http://120.79.207.54:5666/user/stu_confirm',{
          phone:this.phone,
          school:this.school,
          stu_id:this.stu_id,
          sesson_id:session_id
        })
        .then(res=>{
          if(res.data.Message=='success'){
            Toast({
              message:'认证成功',
              duration:1000
            })
            setTimeout(()=>{
              this.$router.push({
                path: 'my'
              })
          },1000)
          }else {
            Toast({
              message:'认证失败,请检查认证信息是否无误',
              duration:1000
            })
            return;
          }
        })
        .catch(err=>{
          Toast({
            message:'似乎与外界断了联系，再试试吧~',
            duration:1000
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

.authenty{
  position: absolute;
  top:0;
  width: 100%;
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

.info{
  position: absolute;
  top:6rem;
  left: 1rem;
}

.phone_number,.id_number,.id_password,.this_password,.confirm{
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

.id_number{
  top:4rem;
}

.id_password{
  top:8rem;
}

.this_password{
  top:12rem;
}

.confirm{
    top:23rem;
    position: absolute;
    width: 20.5rem;
    height: 3rem;
    left: 1rem;
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
