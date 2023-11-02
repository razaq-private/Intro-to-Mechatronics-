/*
 * HTML and Javascript code
 */
const char body[] PROGMEM = R"===(
<!DOCTYPE html>
<html><body>


<input type="range" min="1" max="100" value="50"  id="frequency">
<span id="outputlabel">  </span> <br>

<input type="range" min="0" max="100" value="50"  id="duty">
<span id="outputlabel1">  </span> <br>

</body>
<script>

frequency.onchange = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("outputlabel").innerHTML = this.responseText;
    }
  };
  var str = "frequency?val=";
  var res = str.concat(this.value);
  xhttp.open("GET", res, true);
  xhttp.send();
}

</script>

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

</html>
)===";
