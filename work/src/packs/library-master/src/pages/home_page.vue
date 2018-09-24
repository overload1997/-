<template>
  <div class="home_page">
      <div class="head">
          <span class="head_word">小懂首页</span>
          <i class="icon iconfont search" @click="search">&#xe601;</i>
      </div>
      <div class="notice">
          <span class="word1">公告</span>
          <div class="commit1">还书了吗？来评个分吧？</div>
      </div>
      <div class="guess-you-like">
          <div class="like"></div>
          <span class="word5">猜你喜欢</span>
          <div class="border5"></div>
          <swipe class="slide_book"></swipe>
      </div>
      <div class="recommend-to-you">
          <div class="recommend"></div>
          <span class="word7">为你推荐</span>
          <div class="recommend_book">
              <i class="icon iconfont hot_book">&#xe60e;</i>
              <span class="hot_word">热门书籍<br>推荐</span>
              <i class="icon iconfont classic_book">&#xe60f;</i>
              <span class="classic_word">经典书籍<br>推荐</span>
          </div>
      </div>
      <div class="book-situation">
          <div class="situation"></div>
          <span class="word6">图书情况</span>
          <div class="book-table">
              <div class="book-table-1">
                  <div class="border2"></div>
                  <span class="word2">每日借书人数</span>
                  <daily_borrow class="column"></daily_borrow>
              </div>
              <div class="book-table-2">
                  <div class="border3"></div>
                  <span class="word3">今日借书类别占比</span>
                  <borrow_classify class="circle"></borrow_classify>
              </div>
              <div class="book-table-3">
                  <div class="border4"></div>
                  <span class="word4">各类书的一周借阅时间总和</span>
                  <borrow_time class="graph"></borrow_time>
              </div>
          </div>
      </div>
      <div class="white"></div>
      <page_bottom class="bottom" v-on="page_start"></page_bottom>
  </div>
</template>

<script>
import page_bottom from '../components/page_bottom'
import daily_borrow from '../components/daily_borrow'
import borrow_classify from '../components/borrow_classify'
import borrow_time from '../components/borrow_time'
import swipe from '../components/swipe'
import search from '@/pages/search'
// import func from './vue-temp/vue-editor-bridge';
export default {
  data(){
    return {
      friend:0,
      home:0,
      my:0
    }
  },
  components:{
    page_bottom,
    daily_borrow,
    borrow_classify,
    borrow_time,
    swipe
  },
  methods:{
    search:function(){
      this.$router.push({path:'search'})
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

.home_page{
  position: fixed;
  width:100vw;
  height: 100vh;
}

.head{
  position: fixed;
  top:0;
  left: 0;
  height: 1.555556rem;
  width: 100vw;
  z-index:5;
  background: linear-gradient(to right, #57BAD9 18%, #2593AE 100%);
  -moz-box-shadow:0 0 20px #c0c2c4;
  -webkit-box-shadow:0 0 20px #c0c2c4;
  box-shadow:0 0 20px #c0c2c4;
}

.head_word{
  position: fixed;
  left: 1.722222rem;
  top:.444444rem;
  width: 7.611111rem;
  height: .805556rem;
  z-index:5;
  font-family: NotoSansCJKsc-Regular;
  font-size: .555556rem;
  color: #FFFFFF;
  letter-spacing: 0;
}

.search{
  position: fixed;
  z-index:5;
  left:8.805556rem;
  top:.444444rem;
  width: .666667rem;
  height: .666667rem;
  color: white;
  font-size:.555556rem;
}

.notice{
  position: fixed;
  top:1.75rem;
  height: .888889rem;
  width: 100%;
  border-top: solid 1px #979797;
  border-bottom: solid 1px #979797;
}

.word1{
  position: fixed;
  left: .444444rem;
  top: 1.916667rem;
  width: 1.222222rem;
  height: .555556rem;
  font-family: NotoSansCJKsc-Regular;
  font-size: .444444rem;
  color: #5E5E5E;
}

.commit1{
  position: fixed;
  left: 2.222222rem;
  top:1.916667rem;
  width: 12.5rem;
  height: .555556rem;
  font-family: NotoSansCJKsc-Regular;
  font-size:  .444444rem;
  color: #3F3E3E;
}

.guess-you-like{
  position: fixed;
  width: 9.111111rem;
  height: 5.388889rem;
  top:2.833333rem;
  left: .444444rem;
}

.like{
  position: fixed;
  left: .444444rem;
  top: 2.833333rem;
  width: 9.111111rem;
  height: .666667rem;
  border: 0 white;
  border-radius:4px;
  z-index:-1;
  background-color: #57BAD9;
  -moz-box-shadow:0 0 20px #c0c2c4;
  -webkit-box-shadow:0 0 20px #c0c2c4;
  box-shadow:0 0 20px #c0c2c4;
}

.word5{
  position: absolute;
  top:0.1rem;
  left: 2.5rem;
  width: 4.444444rem;
  height: .666667rem;
  font-family: NotoSansCJKsc-Medium;
  font-size: .416667rem;
  z-index: 0;
  color: #FFFFFF;
  letter-spacing: 1px;
  text-align: center;
}

.border5{
  position: fixed;
  left:.444444rem;
  top:8.055556rem;
  width: 9.111111rem;
  /* height: 4.5rem; */
  border: 1px solid #D8F0F7;
}

.slide_book{
  position: fixed;
  left:.444444rem;
  top:3.722222rem;
  width: 9.111111rem;
  height: 4.5rem;
}

.recommend-to-you{
  position: fixed;
  width: 9.5rem;
  top:8.444444rem;
}

.recommend{
  position: absolute;
  left: .416667rem;
  width: 9.111111rem;
  height: .666667rem;
  border: 0 white;
  border-radius:4px;
  background-color: #57BAD9;
  -moz-box-shadow:0 0 20px #c0c2c4;
  -webkit-box-shadow:0 0 20px #c0c2c4;
  box-shadow:0 0 20px #c0c2c4;
}

.word7{
  position: fixed;
  top:8.444444rem;
  left: .416667rem;
  width: 9.111111rem;
  height: .666667rem;
  font-family: NotoSansCJKsc-Medium;
  font-size: .5rem;
  color: #FFFFFF;
  letter-spacing: .027778rem;
  text-align: center;
}

.hot_book,.classic_book{
  position: fixed;
  top:9.333333rem;
  width: 4.444444rem;
  height: 2.666667rem;
  z-index: -1;
  font-size:.5rem;
}

.hot_word,.classic_word{
  position: fixed;
  width: 2.888889rem;
  height: 2rem;
  top:9.666667rem;
  font-family: NotoSansCJKsc-Regular;
  font-size: .5rem;
  color: #5E5E5E;
  letter-spacing: .055556rem;
  text-align: center;
}

.hot_word{
  left: 1.222222rem;
}

.hot_book{
  left: 0.8rem;
  color:#FFE3E0;
}

.classic_word{
  left: 5.888889rem;
}

.classic_book{
  left: 5.111111rem;
    color: #DBF0F7;
}

.book-situation{
  position: fixed;
  top: 8.333333rem;
  width: 9.111111rem;
}

.situation{
  position: fixed;
  top: 12.222222rem;
  left: .416667rem;
  width: 9.111111rem;
  height: .666667rem;
  border: 0 white;
  border-radius:4px;
  background-color: #57BAD9;
  -moz-box-shadow:0 0 20px #c0c2c4;
  -webkit-box-shadow:0 0 20px #c0c2c4;
  box-shadow:0 0 20px #c0c2c4;
}

.word6{
  position: fixed;
  top: 12.222222rem;
  left: .416667rem;
  width: 9.111111rem;
  height: .666667rem;
  font-family: NotoSansCJKsc-Medium;
  font-size: .5rem;
  color: #FFFFFF;
  letter-spacing: 1px;
  text-align: center;
}

.book-table{
  position: absolute;
  top:2.2rem;
  width: 22.5rem;
}

.book-table-1{
  position: absolute;
  width: 22.5rem;
  height: 14rem;
}

.book-table-2{
  position: absolute;
  width: 22.5rem;
  top:14.5rem;
  height: 16rem;
}

.book-table-3{
  position: absolute;
  width: 22.5rem;
  top:31rem;
  height: 14rem;
}

.border2,.border4{
  position: absolute;
  left: 1rem;
  width: 20.5rem;
  height: 14rem;
  z-index: -1;
  border: solid 1px #E6E6FA;
}

.word2,.word3,.word4{
  position: fixed;
  left: 3.805556rem;
  /* top:13.111111rem; */
  width: 2.333333rem;
  height: .555556rem;
  text-align: center;
  font-family: NotoSansCJKsc-Regular;
  font-size: .444444rem;
  color: black;
  letter-spacing: .027778rem;
}

.border3{
  position: absolute;
  left: 1rem;
  width: 20.5rem;
  height: 16rem;
  border: solid 1px #E6E6FA;
}

.graph,.circle,.column{
  position: absolute;
  top:2rem;
}

.white{
  position: absolute;
  top:76rem;
  width: 22.5rem;
  height: 3rem;
}

.bottom{
  position: fixed;
  bottom: 0;
}

</style>

