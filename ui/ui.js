goog.provide('friendlyhal');

goog.require('goog');
goog.require('goog.dom');
goog.require('goog.net.XhrIo');
goog.require('goog.Timer');
goog.require('goog.events');

// Commands.
friendlyhal.PAN_LEFT = "/cgi-bin/action/panleft";
friendlyhal.PAN_RIGHT = "/cgi-bin/action/panright";
friendlyhal.PAN_UP = "/cgi-bin/action/panup";
friendlyhal.PAN_DOWN = "/cgi-bin/action/pandown";

friendlyhal.GET_TEMPERATURE_DATA = "/cgi-bin/action/temperature";
friendlyhal.GET_HUMIDITY_DATA = "/cgi-bin/action/humidity";
friendlyhal.GET_MOTION_DATA = "/cgi-bin/action/motion";

friendlyhal.appLog = function(msg) {
};

friendlyhal.start = function() {
    friendlyhal.setupChartRefresh();
};


friendlyhal.pan = function(panEvent) {
    goog.net.XhrIo.send(panEvent);
};

dataToDataTable = function(arr, opt_divBy100) {
    var data = new google.visualization.DataTable();
    data.addColumn('datetime');
    data.addColumn('number');
    for (var i in arr) {
	if (opt_divBy100) {
	    data.addRow([new Date(arr[i][0] * 1000), arr[i][1]/100]);
	} else {
	    data.addRow([new Date(arr[i][0] * 1000), arr[i][1]]);
	}
    }
    return data;
}

friendlyhal.temperatureDataCallback = function(e) {
    var obj = e.target.getResponseJson();
    var chart = new google.visualization.LineChart(
	document.getElementById('temperaturearea'));
    var options = {
	'curveType': 'function',
	'title': 'Temperature',
	'legend': 'none',
	'backgroundColor': 'black',
	'vAxis': { 'title': 'C', 'gridlines': { 'color': '#555' } },
	'hAxis': { 'format': 'hh:mm', 'gridlines': { 'count': 0, 'color': '#555'} }
    };
    chart.draw(dataToDataTable(obj, true), options);
};

friendlyhal.humidityDataCallback = function(e) {
    var obj = e.target.getResponseJson();
    var chart = new google.visualization.LineChart(
	document.getElementById('humidityarea'));
    var options = {
	'curveType': 'function',
	'title': 'Rel. Humidity',
	'legend': 'none',
	'backgroundColor': 'black',
	'vAxis': { 'title': 'Rel. Humidity', 'gridlines': { 'color': '#555' }},
	'hAxis': { 'format': 'hh:mm','gridlines': { 'count': 0 , 'color': '#555'} }
    };
    chart.draw(dataToDataTable(obj, true), options);
};

friendlyhal.motionDataCallback = function(e) {
    var obj = e.target.getResponseJson();
    var chart = new google.visualization.LineChart(
	document.getElementById('movementarea'));
    var options = {
	'curveType': 'function',
	'title': 'Movement',
	'legend': 'none',
	'backgroundColor': 'black',
	'vAxis': { 'title': 'Moves', 'gridlines': { 'color': '#555' }},
	'hAxis': { 'format': 'hh:mm','gridlines': { 'count': 0, 'color': '#555'} }
    };
    chart.draw(dataToDataTable(obj), options);
};

friendlyhal.refreshAllCharts = function() {
    console.log('Refreshing charts...');

    goog.net.XhrIo.send(friendlyhal.GET_TEMPERATURE_DATA,
			friendlyhal.temperatureDataCallback);
    goog.net.XhrIo.send(friendlyhal.GET_MOTION_DATA,
			friendlyhal.motionDataCallback);
    goog.net.XhrIo.send(friendlyhal.GET_HUMIDITY_DATA,
			friendlyhal.humidityDataCallback);
    console.log('Sent out refresh requests.');
};

friendlyhal.setupChartRefresh = function() {
    console.log('Setting up chart refresh.');
    friendlyhal.refreshAllCharts();
    setInterval(function() {
	friendlyhal.refreshAllCharts();
    }, 30000);
    return true;
};