<template>
<div class="slideshow">
  <div class="book">
    <mt-swipe :auto="3000" class="swipe">
    <mt-swipe-item class="swip-item" v-for="book in books" :key="book.id">
      <img class="photo" :src="book.mysrc" />
      <span class="name" >{{book.name}}</span>
      <span class="author">{{book.author}}</span>
      <span class="classify">{{book.classify}}</span>
      <span class="publishing">{{book.publishing}}</span>
    </mt-swipe-item>
    </mt-swipe>
  </div>
  <div class="slidebutton">
    <i class="icon iconfont pre" @click="pre"></i>
    <i class="icon iconfont next" @click="next"></i>
  </div>
</div>
</template>

<script>
export default {
  data(){
      return{
            books:[
              {
                mysrc:'https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1536668727755&di=a5c060cfebd71b848b1e8784d217e199&imgtype=0&src=http%3A%2F%2Fimgsrc.baidu.com%2Fimgad%2Fpic%2Fitem%2Faec379310a55b3191b36b66649a98226cefc17d1.jpg',
                name:'小猪',
                author:'you guess',
                classify:'you guess',
                publishing:'出版社啊'
              },
              {
                mysrc:'http://g3.hexunimg.cn/2015-11-01/180266497.jpg',
                name:'小猪',
                author:'you guess',
                classify:'you guess',
                publishing:'出版社啊'
              },
              {
                mysrc:'http://imgsrc.baidu.com/imgad/pic/item/08f790529822720eabb159e871cb0a46f21faba1.jpg',
                name:'汪',
                author:'you guess',
                classify:'you guess',
                publishing:'出版社啊'
              }
            ]
          }
    },
  methods:{
    pre(){

    },
    next(){

    },
  },
  created(){
    setTimeout(()=>{
    $('.mint-swipe-indicator').removeClass('mint-swipe-indicator').addClass('mint-swipe-indicator');
    $('.mint-swipe-indicator').css('color','black');
    },10);
  }
}
</script>

<style>
/* html{
  position: relative;
  width: 20.5rem;
  font-size: 16px;
} */
.slideshow{
  position: fixed;
  width: 100vw;
}
.book,.swipe,.swipe-item{
  position: fixed;
  width: 100vw;
  /* height: 4.5rem; */
}
/*那个下面的点*/
.mint-swipe-indicators{
  bottom: 0;
  left: 4.555556rem;
  height:0.5rem;
}
div.mint-swipe-indicator{
  color: black!important;
}
.photo{
  position: fixed;
  left: .472222rem;
  top: 3.75rem;
  width: 3.333333rem;
  height: 4.166667rem;
}
.name{
  position: fixed;
  left: 4.027778rem;
  top:4.472222rem;
  width: 5.5rem;
  height: .75rem;
  font-family: NotoSansCJKsc-Medium;
  font-size: .611111rem;
  color: #343434;
  letter-spacing: .027778rem;
}
.author,.classify,.publishing{
  position: fixed;
  left: 4.027778rem;
  width: 5.5rem;
  height: .666667rem;
  font-family: NotoSansCJKsc-Medium;
  font-size: .444444rem;
  color: #343434;
  letter-spacing: .027778rem;
}
.author{
  top:5.194444rem;
}
.classify{
  top:5.861111rem;
}
.publishing{
  top:6.527778rem;
}
</style>
