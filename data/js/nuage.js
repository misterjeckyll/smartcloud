/*jslint vars: true, plusplus: true, devel: true, nomen: true, indent: 4, maxerr: 50*/

'use strict';
function ajaxGET(url, callback) {
    console.log(url);
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            callback.call(this);
        }
    };
    xhttp.open("GET", url, true);
    xhttp.send();
}

function saveWifi(){
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            console.log("wifi settings saved");
        }
    };
    xhttp.open("POST", "/setNetwork", true);
    xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
    let select_ssid = document.getElementById("ssid");
    let ssid = select_ssid.options[select_ssid.selectedIndex].value;
    console.log(ssid);
    let pwd = document.getElementById("pwd").value;
    console.log(pwd);
    xhttp.send("ssid=" + ssid + "&pwd=" + pwd);

}

function scanNetwork() {
    
    ajaxGET("./scanNetwork", function () {
        document.getElementById("ssid").innerHTML = this.responseText;
    });
}

function getSSID() {
    
    ajaxGET("./getSSID", function () {
        document.getElementById("cloudSSID").innerHTML = this.responseText;
    });
}
function reboot() {
    
    ajaxGET("./cloudReboot", function () {});
}



var navItems = document.querySelectorAll(".mobile-bottom-nav .nav-link, .desktop-top-nav .nav-link");

navItems.forEach(function (element, i) {
    
	element.addEventListener("click", function (event) {
		navItems.forEach(function (e2, i2) {
			e2.classList.remove("nav-active");
		});
		this.classList.add("nav-active");
	});
});

document.getElementById("reboot-tab").addEventListener("click", function (event) {
    
    if (!event.target.classList.contains("active")) {
        reboot();
    }
});


scanNetwork();
getSSID();


var colorPicker = new iro.ColorPicker('#picker', {
    wheelLightness: false,
    colors : ["rgb(128, 128, 128)"]
});

colorPicker.setColors(["rgb(128, 128, 128)"]);

colorPicker.on(["mount", "color:setActive", "color:change"], function () {
    for(let element of document.querySelectorAll(".colorList")){
        element.innerHTML = '';
        colorPicker.colors.forEach(color => {
            let col = color.clone();
            if(current != "/randomblink"){
                col.hsv = {h: col.hue, s: col.saturation, v: 100};
            }else{
                col.saturation = 0;
            }  
            element.innerHTML += `<div class="swatch" style="background: ${ col.hexString }"></div>`;
        });
    }
    
});

function sendEffect(effect) {
    var request;
    switch (effect) {
    case '/simple':
        var color = colorPicker.color;
        request = effect.concat("?r=", color.red, "&g=", color.green, "&b=", color.blue);
        break;
    case '/randomblink':
        var lum = colorPicker.color.value;
        request = effect.concat("?v=", lum);
        break;
    case '/snake':
        var colors = colorPicker.colors;
        request = effect.concat("?d=0");
        colors.forEach(color => {
            request = request.concat("&r",color.index,"=",color.red);
            request = request.concat("&g",color.index,"=",color.green);
            request = request.concat("&b",color.index,"=",color.blue);
        });
        break;
    default:
        return;
    }
    ajaxGET(request, function(){});
}

colorPicker.on("input:end",function(color){
    sendEffect(current);
})

var effects = ["randomblink","simple","snake"];
var current = "/randomblink";
document.getElementById("effects").addEventListener('slide.bs.carousel', function (e) {
    current = "/"+effects[e.to];
    switch(current){
        case '/simple':
            colorPicker.setColors(["rgb(15, 71, 128)"]);
            sendEffect(current);
            break;
        case '/randomblink':
            colorPicker.setColors(["rgb(128, 128, 128)"]);
            sendEffect(current);
            break;
        case '/snake':
            colorPicker.setColors(["rgb(15, 71, 128)","rgb(71, 15, 20)"]);
            sendEffect(current);
            break;
    }
});