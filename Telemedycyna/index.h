const char index_html[] PROGMEM = R"webpage(
<!DOCTYPE HTML><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<script src="https://code.highcharts.com/8.0/highcharts.js"></script>
<style>
body {
min-width: 300px;
max-width: 1000px;
height: 400px;
margin: 0 auto;
}
h2 {
font-family: Arial;
font-size: 2.5rem;
text-align: center;
}
</style>
</head>

<body>
<h2>Pressure force monitoring</h2>
<div id="temperature-chart" class="container"></div>
</body>

<script>
var chartT = new Highcharts.Chart({
chart:{ renderTo : 'temperature-chart' },
title: { text: 'Pressure force graph' },
series: [{
showInLegend: false,
data: []
}],
plotOptions: {
line: { animation: false,
dataLabels: { enabled: true }
},
series: { color: '#059e8a' }
},
xAxis: { 
type: 'datetime',
title: { text: 'Time [mm:ss]' },
dateTimeLabelFormats: { second: '%M:%S' }
},
yAxis: {
title: { text: 'Pressure force [kg]' }
},
credits: { enabled: false }
});

var start = new Date();                             //zmienna okreslająca rozpoczęcie działania serwera

setInterval(function ( ) {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {

    var stop = new Date();                          //zmienna określająca czas danego pomiaru

var x = (stop.getTime() - start.getTime()),         //w celu uzyskania oliczania czasu od 0
y = parseFloat(this.responseText);
if(chartT.series[0].data.length > 45) {             //45 punktów na wykresie
chartT.series[0].addPoint([x, y], true, true, true);
} else {
chartT.series[0].addPoint([x, y], true, false, true);
}
}
};
xhttp.open("GET", "/weight", true);
xhttp.send();
}, 400 ) ;                                          //aktualizacja wykresu co 400 ms
</script>
</html>)webpage";