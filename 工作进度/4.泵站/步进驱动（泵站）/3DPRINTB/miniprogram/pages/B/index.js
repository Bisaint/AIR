const app = getApp()
//消息用#号做了包裹，方便字符串分割，不过自己要知道哪个数是表示什么意思就行，可以用其他符号隔开，这里用了#号，不局限，自己设定就行
//假设控制台有消息：#26#99#47.5#27#17#on#   //获取的是控制台消息，故控制台要有传感器数据
Page({
  data: {
  },
  A() {
    wx.navigateTo({
      url: "/pages/B/A/A",
    })
  },
  B() {
    wx.navigateTo({
      url: "/pages/B/B/A",
    })
  },
  onShareAppMessage: function (res) {
    if (res.from === 'button') {
      console.log(res.target)
    }
    return {
      title: "3D控制平台",
      path: "/pages/A/index"
    }
  },
  onShareTimeline: function () {
    return {
      title: "3D控制平台",
      query: {
        key: value
      },
      imageUrl: 'pages/A/index'
    }
  },
  handlerGobackClick() {
    wx.showModal({
      title: '你点击了返回',
      content: '是否确认放回',
      success: e => {
        if (e.confirm) {
          const pages = getCurrentPages();
          if (pages.length >= 2) {
            wx.navigateBack({
              delta: 1
            });
          } else {
            wx.navigateTo({
              url: '/pagesd/MENU/MENU'
            });
          }
        }
      }
    });
  },
  handlerGohomeClick() {
    wx.navigateTo({
      url: '/pagesd/MENU/MENU'
    });
  },
})