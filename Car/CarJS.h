/*
 * Hunter Williams, Sarah Ho, Razaq Aribdesi | MEAM 510 | FALL 2021
 * This file contains HTML and Javascrript to run a webpage on the
 * ESP32. There are two levers  to control motor behaviors and 
 * a slider to control speed.
 * 
 * HTML and Javascript code
 */
 
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
<head> <title> TankMode </title>
    <meta name="viewport" content="user-scalable=no">
    <link rel="shortcut icon" type="image/png"
 href="https://icons.iconarchive.com/icons/glyphish/glyphish/32/136-tractor-icon.png">

 
 <style>
 .button {
  border: none;
  color: white;
  padding 15px 32px;
  text-aligh: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}
.button1{background-color: #4CAF50;} /* Green*/
.button2{background-color: #FF2D00;} /* Red*/
 </style>
 
</head>
<body style="text-align: center;" >

Tank mode: Use Keys [Q] [A] and [P]  [L] for levers. <br>
  <canvas id="canvas"></canvas><br>

  
<p>MOTOR SPEED</p>
<input type="range" min="0" max="100" value="0"  id="speed">
<span id="speedlbl">  </span> <br><br>

<p>Autonomy</p>
<button class="button button1" onclick="beaconTrack()">Track Beacon</button>
<button class="button button1" onclick="canTrack()">Track Can</button>
<button class="button button1" onclick="wallFollow()">Follow Wall</button>
<button class="button button2" onclick="stopAutonomy()">Stop Autonomy</button> <br><br>

<p>POSITION</p>
<span id="x1position"> </span> <span id="y1position"> </span> <br>
<span id="x2position"> </span> <span id="y2position"> </span> <br>

Command Sent: <span id="levers"> </span> <br>
ESP32 Received: <span id="acknowledge">  </span> <br>

<span id="debug"> </span> <br>

<script>
  var canvas, ctx, width, height;
  var leftarmstate=0, rightarmstate=0;
  var leftstate=0, rightstate=0; forward=0;
  var leverstate = [];
  
  canvas = document.getElementById('canvas');
  ctx = canvas.getContext('2d');          
  width = canvas.width;
  height = canvas.height;

/*
// double check for inconsistency 2x a second
  setInterval(myTimer, 800); 
  function myTimer() {  
    if (leftstate != leverstate[2] || rightstate != leverstate[3]) {
      updateState();
      document.getElementById("debug").innerHTML =  "fixing";
    }
  }
 */ 
// keyboard
  document.addEventListener('keydown', keyDownHandler, true);
  document.addEventListener('keyup', keyUpHandler, true);
  window.addEventListener('focus', focusChange);
  window.addEventListener('blur', focusChange);

function focusChange() {
  leftstate=0; rightstate=0;
    leftarmstate=0; rightarmstate=0;
    updateState();
}

var keyboardCode = function(event) {
  var code;
  if (event.repeat) return 0; // ignore repeating if held down
  if (event.key !== undefined) { code = event.key; } 
  else if (event.keyCode !== undefined) {code = event.keyCode;}
  return code;
};

drawLevers();

function updateState(){
  drawLevers();
  sendState();
}

function keyDownHandler(event) {
  var code = keyboardCode(event);
    if (code == 0) return; // repeating
    document.getElementById("debug").innerHTML =  code;
      
    if(code == 81 || code == 'q') { // Q key
      leftstate = -4;
    }
    if(code == 65 || code == 'a') { // A key
      leftstate = 4;
    }
    if(code == 80 || code == 'p') { // P key
      rightstate = -4;
    }
    if(code == 76 || code == 'l') { // L key 
      rightstate = 4;
    }
    if(code == 98 || code == 'b') {
      forward = 4;
    }
    if(code == 112 || code == 'y') {
      forward = -4;
    }
    
    updateState();
}
//Motor Speed
speed.onchange = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("speedlbl").innerHTML = this.responseText;
    }
  };
  var str = "speed?val=";
  var res = str.concat(this.value);
  xhttp.open("GET", res, true);
  xhttp.send();
}

//Beacon Tracking
function beaconTrack() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "beaconTrack", true);
  xhttp.send();
}
//Can Tracking
function canTrack() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "canTrack", true);
  xhttp.send();
}
//Wall Following
function wallFollow() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "wallFollow", true);
  xhttp.send();
}
//Stop Autonomy
function stopAutonomy() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "stopAutonomy", true);
  xhttp.send();
}

function keyUpHandler(event) {
  var code =keyboardCode(event);
    
    if(code == 81 || code == 'q' ) { // Q key
      leftstate = 0;
    }    
    if(code == 65 || code == 'a' ) { // A key
      leftstate = 0;
    }
    if(code == 80 ||  code == 'p' ) { // P key
      rightstate = 0 ; 
    }
    if(code == 76 ||  code == 'l' ) { // L key
      rightstate =0  ; 
    }
    if(code ==98 || code =='b') {
      forward = 0;
    }
    if(code == 121 || code =='y') {
      forward = 0;
    }
    
    updateState();
}

// DRAWING functions
  function background() {
    ctx.beginPath();
    ctx.fillStyle = '#c5c5c5';
    ctx.rect(width*1/4-5, height/2-50, 10, 100);
    ctx.rect(width*3/4-5, height/2-50, 10, 100);
    ctx.fill();
    ctx.fillStyle = 'black';
    ctx.fillText("[ Q ]",width*1/4-8, height/2-55);
    ctx.fillText("[ A ]",width*1/4-8, height/2+60);
    ctx.fillText("[ P ]",width*3/4-8, height/2-55);
    ctx.fillText("[ L ]",width*3/4-8, height/2+60);
  }
  
  function knob(x,y) {
    var grd = ctx.createRadialGradient(x+10,y-10,3,x+10,y-10,50);
    ctx.beginPath();
    grd.addColorStop(0,"white");
    grd.addColorStop(1,"red");
    ctx.fillStyle = grd;
    ctx.arc(x, y, 20, 0, Math.PI * 2, true);
    ctx.fillStyle = grd;
    ctx.fill();
  }
  
  function drawLevers() {
    ctx.clearRect(0, 0, width, height);
    background();  
    knob(width*1/4, height/2+leftstate*10); // left knob
    knob(width*3/4, height/2+rightstate*10); // right knob
  }
  
// UI functions via AJAX to esp32
  function sendState() {
    var xhttp = new XMLHttpRequest();
    var str = "lever?val=";
    var res = str.concat(leftstate,",",rightstate,",",forward);
    document.getElementById("levers").innerHTML = res; 
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4) {
        if (this.status == 200) {
          resendflag = 0;         
          leverstate = this.responseText.split(",");
          document.getElementById("acknowledge").innerHTML = this.responseText;
          document.getElementById("xposition").innerHTML = this.responseText;
          document.getElementById("yposition").innerHTML = this.responseText;
        }  else  resendflag = 1
      }
    };
    xhttp.open("GET", res, true);
    xhttp.send();
  }
</script>
</body>
</html>
)===";
