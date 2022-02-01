function ajaxGET(url, callback, error = undefined) {
    console.log(url);
    let xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            if(callback !== undefined) callback.call(this);
        }else if(this.readyState === 4 && this.status !== 200){
            if(error !== undefined) error.call(this);
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
    ajaxGET("./cloudReboot");
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
    reboot();
});


scanNetwork();
getSSID();


var colorPicker = new iro.ColorPicker('#picker', {
    wheelLightness: false,
    width:310,
    display:"inline-block",
    handleRadius : 9,
    activeHandleRadius : 11,
    colors : ["rgb(128, 128, 128)"]
});
let container = colorPicker.base;
console.log(container);

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
    ajaxGET(request);
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


for(let elm of document.querySelectorAll(".form-switch input")){
    elm.addEventListener("change",function(e){
        let state = elm.checked;
        var request = "/";
        request = request.concat(elm.id, "?state=", elm.checked ?1:0);
        ajaxGET(request, function(){
        }, function(){
            elm.checked = !state;
            let toa = document.getElementById("toastdisconnected");
            var toast = new bootstrap.Toast(toa);
            toast.show();
        });
    })
}
window.addEventListener('load', function () {
    if(window.matchMedia("(max-width: 751px)").matches){
        console.log("small screen mode");
    }else{
        console.log("large screen mode");
        
        let lum = document.getElementById("lum");
        let lumparent = document.getElementById("lumparent");
        let width = lumparent.clientHeight;
        console.log(width + "px");
        lum.style.width = width + "px";
        lum.style.transformOrigin = width/2 + "px " + width/2 + "px";
    }
    
})


/*
 drag button interact
*/
interact('.draggable')
  .draggable({
    // enable inertial throwing
    inertia: true,
    startAxis:'y',
    // keep the element within the area of it's parent
    modifiers: [
      interact.modifiers.restrictRect({
        restriction: 'parent',
        endOnly: true
      })
    ],
    // enable autoScroll
    autoScroll: true,
    lockAxis: 'y',
    listeners: {
      // call this function on every dragmove event
      move: dragMoveListener,

      // call this function on every dragend event
      end (event) {
        var textEl = event.target.querySelector('p')
      }
    }
  })

function dragMoveListener (event) {
    var target = event.target

    // keep the dragged position in the data-x/data-y attributes
    let x = (parseFloat(target.getAttribute('data-x')) || 0) + event.dx
    let y = (parseFloat(target.getAttribute('data-y')) || 0) + event.dy
    let fullheight = document.getElementById("slider").clientHeight;
    
    let percent = Math.round((y/(fullheight-target.clientHeight))*1000000)/10000;
    var secondsInADay = 24 * 60 * 60;
    let numberofseconds = Math.round(percent * secondsInADay / 100);
    let selectedTime = new Date();
    selectedTime.setUTCHours(-1,0,0,0);
    
    
    selectedTime.setSeconds(numberofseconds);
    let timestring = selectedTime.toLocaleTimeString({hour:'2-digit'});
    target.querySelector(".time").textContent = timestring.slice(0,-3);

    // translate the element
    target.style.transform = 'translate(' + x + 'px, ' + y + 'px)'

    // update the posiion attributes
    target.setAttribute('data-x', x)
    target.setAttribute('data-y', y)
    
    
}