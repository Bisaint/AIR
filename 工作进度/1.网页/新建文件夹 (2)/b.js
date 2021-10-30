//获取cookie代码
var getCookie = function(name) {
    var arr;
    var reg = new RegExp("(^| )" + name + "=([^;]*)(;|$)");
    if (arr = document.cookie.match(reg)) {
        return arr[2];
    } else
        return null;
};
//点击获取按钮之后调用的函数
function getcookie() {
    console.log(getCookie("userName"));
    console.log(getCookie("password"))
}