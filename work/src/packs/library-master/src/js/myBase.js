/**
 * 主要是一些基本的方法 例如：登录的非空判断  手机号码正则  注册时候的两次密码是否一致性 等等。。。。
 * 都是一些toast的方法
 */
import {Toast} from 'mint-ui'
//登录界面的输入合法
function Init(){

}

//手机正则判断
export const checkPhone=function(phoneInput){ 
    var myreg=/^[1][3,4,5,7,8][0-9]{9}$/;
    if (!myreg.test(phoneInput)) {
        return false;
    } else {
        return true;
    }
}
