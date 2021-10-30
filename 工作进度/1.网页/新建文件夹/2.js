var data = [{
    id: 1,
    name: '低聚物5'
}, {
    id: 2,
    name: '低聚物2'
}, {
    id: 3,
    name: '低聚物3'
}];
var provinceEle = document.getElementById("province");
for (let i = 0; i < data.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle.add(new Option(data[i].name, data[i].id));
}
var oBtn = document.getElementById('btn');
var oSel = document.getElementById('sel');
oBtn.onclick = function() {
    var oNewSel = oSel.cloneNode(true);
    document.body.appendChild(oNewSel);
};