import Vue from 'vue'
import vuex from 'vuex'
Vue.use(vuex);

export default new vuex.Store({
    state:{
        isLogin:false,
        SessionId:'',
        Timestamp:'',
        identify_code:'',
        phone:'',
    },
    mutations:{
        changeInfo(state,userInfo){
            state.isLogin=true;
            state.SessionId=userInfo.SessionId;
            state.Timestamp=userInfo.Timestamp;
            // state.username=username;
            console.log('veux:'+state.isLogin);
        },
        //修改密码和忘记密码的都保留在这里
        saveIdentityCodeAndPhone(state,info){
            // alert(JSON.stringify(info));
            // alert(info.identify_code);
            state.identify_code=info.identify_code;
            state.phone=info.phone;
            console.log('vuex='+state.phone);
            console.log('vuex='+state.identify_code);
        }
    }
})