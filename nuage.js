function ajaxGET(url, callback) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            callback.call(this);
        }
    };
    xhttp.open("GET", url, true);
    xhttp.send();
}

function scanNetwork() {
    ajaxGET("./scanNetwork", function () {
        document.getElementById("ssid").innerHTML = this.responseText;
    });
}

function getSSID() {
    ajaxGET("./getSSID", function () {
        document.getElementById("ballSSID").innerHTML = this.responseText;
    });
}
function reboot() {
    ajaxGET("./cloudReboot", function (){
        if(this.status == 200) {
            location.reload();
        }
    })
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

document.getElementById("reboot-tab").addEventListener("click", function (event){
    if(!event.target.classList.contains("active")){
        reboot();
    }
})


scanNetwork();
getSSID();


var colorPicker = new iro.ColorPicker('#picker', {
    wheelLightness:false,
    color : "#1E90FF"
});

const colorList = document.getElementById("colorList");
const activeColor = document.getElementById("activeColor");

function setColor(colorIndex) {
  // setActiveColor expects the color index!
  colorPicker.setActiveColor(colorIndex);
}

colorPicker.on(["mount", "color:setActive", "color:change"], function(){
  // colorPicker.color is always the active color
  const index = colorPicker.color.index;
  const hexString = colorPicker.color.hexString;
  activeColor.innerHTML = `
    <div class="swatch" style="background: ${ hexString }"></div>`;
})

colorPicker.on("input:end",function(color){
    console.log(color.red, color.green, color.blue);
    var request = "/simple?";
    request = request.concat("r=",color.red,"&g=", color.green, "&b=", color.blue);
    ajaxGET(request, function (){
        console.log("color send");
    })
})
