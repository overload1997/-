<template>
  <div id="app">
    <router-view></router-view>
  </div>
</template>

<script>
export default {
  name: 'App'
}
</script>

<style lang="scss">
@import './base/css/normalize.scss';
body{
	background: #f1f8f7;
	#app{
		background: #f1f8f7;
		height:100%;
		font-family: Microsoft YaHei;
	}	

	.swiper-pagination-bullet{
		width:8px;
		height:8px;
		[data-dpr="2"] & {
	    	width:16px;
			height:16px;
	    }
	    [data-dpr="3"] & {
         	width:24px;
     		height:24px;
	    }
	}
}
</style>
