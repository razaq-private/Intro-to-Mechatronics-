/*
 * HTML and Javascript code
 */
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
  <body>
   
    <button class="button button1" onclick="forward()">FORWARD</button>
    <button class="button button2" onclick="reverse()">REVERSE</button>

    <input type="range" min="0" max="100" value="50"  id="duty">
    <span id="outputlabel1">  </span> <br>
   
  </body>

/*<script>

  speed.onchange = function() {
    var xhtttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("speedLabel").innerHTML = this.responseText;
      }
    };
    var str = "speed?val=";
    var res = str.concat(this.value);
    xhttp.open("GET", res, true);
    xhttp.send();
  }
</script>*/

<script>
  duty.onchange = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("outputlabel1").innerHTML = this.responseText;
    }
  };
  var str = "duty?val=";
  var res = str.concat(this.value);
  xhttp.open("GET", res, true);
  xhttp.send();
  }

</script>
  
<script>
  function forward() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "forward", true);
    xhttp.send();
  }

</script>

<script>
  function reverse() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "reverse", true);
    xhttp.send();
  }
</script>

</html>
)===";
