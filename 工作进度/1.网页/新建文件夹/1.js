var copy0 = function() {
    var tmp = document.createDocumentFragment();
    var div = tmp.appendChild(document.createElement("div"));
    var sel = document.getElementById("sel0");
    div.innerHTML = sel.outerHTML;
    var cloned = div.firstChild;
    document.body.insertBefore(cloned, sel.nextElementSibling);
}
var copy1 = function() {
    var tmp = document.createDocumentFragment();
    var div = tmp.appendChild(document.createElement("div"));
    var sel = document.getElementById("sel1");
    div.innerHTML = sel.outerHTML;
    var cloned = div.firstChild;
    document.body.insertBefore(cloned, sel.nextElementSibling);
}
var copy2 = function() {
    var tmp = document.createDocumentFragment();
    var div = tmp.appendChild(document.createElement("div"));
    var sel = document.getElementById("sel2");
    div.innerHTML = sel.outerHTML;
    var cloned = div.firstChild;
    document.body.insertBefore(cloned, sel.nextElementSibling);
}
var copy3 = function() {
    var tmp = document.createDocumentFragment();
    var div = tmp.appendChild(document.createElement("div"));
    var sel = document.getElementById("sel3");
    div.innerHTML = sel.outerHTML;
    var cloned = div.firstChild;
    document.body.insertBefore(cloned, sel.nextElementSibling);
}
var copy4 = function() {
    var tmp = document.createDocumentFragment();
    var div = tmp.appendChild(document.createElement("div"));
    var sel = document.getElementById("sel4");
    div.innerHTML = sel.outerHTML;
    var cloned = div.firstChild;
    document.body.insertBefore(cloned, sel.nextElementSibling);
}

var data0 = [{
    id: 0,
    name: '低聚物1',
    cps: '3000',
}, {
    id: 1,
    name: '低聚物2',
    cps: '5000',
}, {
    id: 2,
    name: '低聚物3',
    cps: '8000',
}];
var provinceEle0 = document.getElementById("province0");
for (let i = 0; i < data0.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle0.add(new Option(data0[i].name, data0[i].id));
}

function show_input() {
    var select_value = document.getElementById('province0').value;
    var cps = document.getElementById("cps0");
    cps.value = "cps:" + data0[select_value].cps;
}
/*
document.getElementById("province0").addEventListener("change", function() {
    var cps = document.getElementById("cps0");
    cps.innerHTML = "cps:" + data0[this.value].cps;
});
*/
var data1 = [{
    id: 1,
    name: 'ACMO'
}, {
    id: 2,
    name: '单体（稀释剂）2'
}, {
    id: 3,
    name: '单体（稀释剂）3'
}];
var provinceEle1 = document.getElementById("province1");
for (let i = 0; i < data1.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle1.add(new Option(data1[i].name, data1[i].id));
}

var data2 = [{
    id: 1,
    name: 'irgacure819'
}, {
    id: 2,
    name: 'TPO'
}, {
    id: 3,
    name: '单体（稀释剂）3'
}];
var provinceEle2 = document.getElementById("province2");
for (let i = 0; i < data2.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle2.add(new Option(data2[i].name, data2[i].id));
}

var data3 = [{
    id: 1,
    name: '颜料（色浆）'
}, {
    id: 2,
    name: '散射剂'
}, {
    id: 3,
    name: '吸收剂'
}];
var provinceEle3 = document.getElementById("province3");
for (let i = 0; i < data1.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle3.add(new Option(data3[i].name, data3[i].id));
}

var data4 = [{
    id: 1,
    name: 'ZrO2'
}, {
    id: 2,
    name: '单体（稀释剂）2'
}, {
    id: 3,
    name: '单体（稀释剂）3'
}];
var provinceEle4 = document.getElementById("province4");
for (let i = 0; i < data4.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle4.add(new Option(data4[i].name, data4[i].id));
}

var data5 = [{
    id: 1,
    name: 'BYK-111分散剂'
}, {
    id: 2,
    name: '单体（稀释剂）2'
}, {
    id: 3,
    name: '单体（稀释剂）3'
}];
var provinceEle5 = document.getElementById("province5");
for (let i = 0; i < data5.length; i++) {
    //new Option() 参数为选项的内容，参数2为选项的value
    provinceEle5.add(new Option(data5[i].name, data5[i].id));
}