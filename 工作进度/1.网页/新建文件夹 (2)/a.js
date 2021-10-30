//设置cookie
var setCookie = function(name, value, day) {
    //当设置的时间等于0时，不设置expires属性，cookie在浏览器关闭后删除
    var expires = day * 24 * 60 * 60 * 1000;
    var exp = new Date();
    exp.setTime(exp.getTime() + expires);
    document.cookie = name + "=" + value + ";expires=" + exp.toUTCString();
};
//删除cookie
var delCookie = function(name) {
    setCookie(name, ' ', -1);
};
//传递cookie
function login() {
    var name = document.getElementById("userName");
    var pass = document.getElementById("passwords");
    setCookie('userName', name.value, 7)
    setCookie('password', pass.value, 7);
    location.href = 'b.html'
}

function deletecookie() {
    delCookie('userName', ' ', -1)
}