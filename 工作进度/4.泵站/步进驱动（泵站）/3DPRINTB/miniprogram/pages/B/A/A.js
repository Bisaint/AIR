const app = getApp()
//消息用#号做了包裹，方便字符串分割，不过自己要知道哪个数是表示什么意思就行，可以用其他符号隔开，这里用了#号，不局限，自己设定就行
//假设控制台有消息：#26#99#47.5#27#17#on#   //获取的是控制台消息，故控制台要有传感器数据
Page({
  data: {
    uid: '22ce0563f226aaac9cf0a7db0ff7cef1', //用户密钥，控制台获取
    STAA: "3DPRINTA",//主题名称 
    TOPICSTA:"3DPRINTSTA",
    A_status: "离线", //默认离线，自定义变量名称，随意命名
    XS:"0",
    XK:"0",
    YS:"0",
    YK:"0",
    MC:"0",
    MS:"0",
    XXA:"0",
    YYA:"0",
    ZZA:"0",
    itemsX: [
      { name: 'X正转', value: '1' },
      { name: 'X反转', value: '0', checked: 'true' },
    ],
    itemsY: [
      { name: 'Y正转', value: '1' },
      { name: 'Y反转', value: '0', checked: 'true' },
    ],
    radioStrX: '0',
    radioStrY: '0',
  },
  radioChangeX: function (e) {
    var str = null;
    for (var value of this.data.itemsX) {
      if (value.value === e.detail.value) {
        str = value.value;
        break;
      }
    }
    this.setData({ radioStrX: str });
  },
  radioChangeY: function (e) {
    var str = null;
    for (var value of this.data.itemsY) {
      if (value.value === e.detail.value) {
        str = value.value;
        break;
      }
    }
    this.setData({ radioStrY: str });
  },
  AS: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      XS: res.detail.value//赋值给name_value
    })
  },
  AK: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      XK: res.detail.value//赋值给name_value
    })
  },
  BS: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      YS: res.detail.value//赋值给name_value
    })
  },
  BK: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      YK: res.detail.value//赋值给name_value
    })
  },
  MSC: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      MC: res.detail.value//赋值给name_value
    })
  },
  MSS: function(res){
    console.log("输入的值为："+res.detail.value);//打印输入的值
    this.setData({
      MS: res.detail.value//赋值给name_value
    })
  },
  UPDAA: function() {

    //当点击打开按钮，更新开关状态为打开
  var that = this

      //控制接口
      wx.request({
        url: 'https://api.bemfa.com/api/device/v1/data/1/', //api接口，详见接入文档
        method:"POST",
        data: {  //请求字段，详见巴法云接入文档，http接口
          uid: that.data.uid,
          topic: that.data.STAA,
          msg:"{XS:"+that.data.XS+",XK:"+that.data.XK+",YS:"+that.data.YS+",YK:"+that.data.YK+",xx:"+that.data.radioStrX+",yy:"+that.data.radioStrY+",MC:"+that.data.MC+",MS:"+that.data.MS+"}",   //发送消息为on的消息
        },
        header: {
          'content-type': "application/x-www-form-urlencoded"
        },
        success (res) {
          console.log(res.data)
          wx.showToast({
            title:'上传成功',
            icon:'success',
            duration:1000
          })
        }
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
              url: '/pages/A/index'
            });
          }
        }
      }
    });
  },
  handlerGohomeClick() {
    wx.reLaunch({
      url: '/pages/A/index'
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
      url: 'https://api.bemfa.com/api/device/v1/data/1/', //get接口，详见巴法云接入文档
      data: {
        uid: that.data.uid,
        topic: that.data.TOPICSTA,
      },
      header: {
        'content-type': "application/x-www-form-urlencoded"
      },
      success (res) {
          console.log(res.data)
          if(res.data.msg.indexOf("#") != -1){//如果数据里包含#号，表示获取的是传感器值，因为单片机上传数据的时候用#号进行了包裹
            //如果有#号就进行字符串分割
            var all_data_arr = res.data.msg.split("#"); //分割数据，并把分割后的数据放到数组里。
            console.log(all_data_arr)//打印数组
            that.setData({ //数据赋值给变量
              XXA:all_data_arr[1],//赋值温度
              YYA:all_data_arr[2], //赋值湿度
              ZZA:all_data_arr[3],//赋值℉温度
            })
          }

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
        url: 'https://api.bemfa.com/api/device/v1/data/1/', //get接口，详见巴法云接入文档
        data: {
          uid: that.data.uid,
          topic: that.data.TOPICSTA,
        },
        header: {
          'content-type': "application/x-www-form-urlencoded"
        },
        success (res) {
            console.log(res.data)
            if(res.data.msg.indexOf("#") != -1){//如果数据里包含#号，表示获取的是传感器值，因为单片机上传数据的时候用#号进行了包裹
              //如果有#号就进行字符串分割
              var all_data_arr = res.data.msg.split("#"); //分割数据，并把分割后的数据放到数组里。
              console.log(all_data_arr)//打印数组
              that.setData({ //数据赋值给变量
                XXA:all_data_arr[1],//赋值温度
                YYA:all_data_arr[2], //赋值湿度
                ZZA:all_data_arr[3],//赋值℉温度
              })
            }
  
        }
      })
    }, 1000)
  }
})