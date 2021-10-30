const app = getApp()
//消息用#号做了包裹，方便字符串分割，不过自己要知道哪个数是表示什么意思就行，可以用其他符号隔开，这里用了#号，不局限，自己设定就行
//假设控制台有消息：#26#99#47.5#27#17#on#   //获取的是控制台消息，故控制台要有传感器数据
Page({
  data: {
    uid: '22ce0563f226aaac9cf0a7db0ff7cef1', //用户密钥，控制台获取
    STAA: "3DPRINTA",//主题名称 
    STAB: "3DPRINTB",//主题名称 
    STAC: "3DPRINTC",//主题名称 
    A_status: "离线", //默认离线，自定义变量名称，随意命名
    B_status:"离线",
    C_status:"离线",
    K:0,
  },
  SB() {
    wx.navigateTo({
      url: "/pages/A/A/A",
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
  onLoad: function () {
    var that = this

    //请求设备状态
    //设备断开不会立即显示离线，由于网络情况的复杂性，离线1分钟左右才判断真离线
    wx.request({
      url: 'https://api.bemfa.com/api/device/v1/status/', //状态api接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.STAA,
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
        console.log(res.data)
        if(res.data.status === "online"){
          that.setData({
            A_status:"在线",
            K:that.data.K+1
          })
        }else{
          that.setData({
            A_status:"离线",
          })
        }
        console.log(that.data.A_status)
      }
    })

    wx.request({
      url: 'https://api.bemfa.com/api/device/v1/status/', //状态api接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.STAB,
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
        console.log(res.data)
        if(res.data.status === "online"){
          that.setData({
            B_status:"在线",
            K:that.data.K+1
          })
        }else{
          that.setData({
            B_status:"离线",
          })
        }
        console.log(that.data.B_status)
      }
    })

    wx.request({
      url: 'https://api.bemfa.com/api/device/v1/status/', //状态api接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.STAC,
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
        console.log(res.data)
        if(res.data.status === "online"){
          that.setData({
            C_status:"在线",
            K:that.data.K+1
          })
        }else{
          that.setData({
            C_status:"离线",
          })
        }
        console.log(that.data.C_status)
      }
    })
    setInterval(function () {
      console.log("定时请求设备状态,默认1秒");
      wx.request({
        url: 'https://api.bemfa.com/api/device/v1/status/',  //get 设备状态接口，详见巴法云接入文档
        data: {
          uid: that.data.uid,
          topic: that.data.STAA,
        },
        header: {
          'content-type': "application/x-www-form-urlencoded"
        },
        success (res) {
          console.log(res.data)
          if(res.data.status === "online"){
            that.setData({
              A_status:"在线"
            })
          }else{
            that.setData({
              A_status:"离线"
            })
          }
          console.log(that.data.A_status)
        }
      })
      wx.request({
        url: 'https://api.bemfa.com/api/device/v1/status/',  //get 设备状态接口，详见巴法云接入文档
        data: {
          uid: that.data.uid,
          topic: that.data.STAB,
        },
        header: {
          'content-type': "application/x-www-form-urlencoded"
        },
        success (res) {
          console.log(res.data)
          if(res.data.status === "online"){
            that.setData({
              B_status:"在线"
            })
          }else{
            that.setData({
              B_status:"离线"
            })
          }
          console.log(that.data.B_status)
        }
      })
      wx.request({
        url: 'https://api.bemfa.com/api/device/v1/status/',  //get 设备状态接口，详见巴法云接入文档
        data: {
          uid: that.data.uid,
          topic: that.data.STAC,
        },
        header: {
          'content-type': "application/x-www-form-urlencoded"
        },
        success (res) {
          console.log(res.data)
          if(res.data.status === "online"){
            that.setData({
              C_status:"在线"
            })
          }else{
            that.setData({
              C_status:"离线"
            })
          }
          console.log(that.data.C_status)
        }
      })
    }, 1000)
  }
})