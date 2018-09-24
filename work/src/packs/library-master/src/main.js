// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
/* eslint-disable */
import Vue from 'vue'
import App from './App'
import router from './router'
import MintUI from 'mint-ui'
import echarts from 'echarts'
import 'mint-ui/lib/style.css'
import { Swipe, SwipeItem } from 'vue-swipe'  // 加这里
import axios from 'axios'
import store from './store'
import './js/base.js'  //淘宝的适配

Vue.component('swipe', Swipe)  // 加这里
Vue.component('swipe-item', SwipeItem) // 加这里
Vue.config.productionTip = false
Vue.prototype.$echarts=echarts;
Vue.use(MintUI)
Vue.prototype.axios=axios;

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  store,
  components: { App },
  template: '<App/>'
})
