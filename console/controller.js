/*
 * The controller pings the server every 5 seconds to get the next
 * view.
 */
controller = function() {
    return this;
};

controller.init = function() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.responseType = "json";
    xmlhttp.onreadystatechange = function() {
	if (xmlhttp.readyState==4 && xmlhttp.status==200)
	{
	    var targetUrl = xmlhttp.response['target_url'];
	    controller.loadFrame(targetUrl);
	}
    }
    xmlhttp.open("GET", "test1.html", true);
    xmlhttp.send();
};

controller.loadFrame_ = function(url) {
    document.getElementById('content').src = url;
};
