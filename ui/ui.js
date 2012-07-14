goog.provide('friendlyhal.ControlPanel');

goog.require('goog');
goog.require('goog.dom');
goog.require('goog.net.XhrIo');

/**
 * @constructor
 */
friendlyhal.ControlPanel = function() {
    this.urlToFunctionMap = {
	'/logs': goog.bind(this.updateLogs, this)
    };
};

friendlyhal.ControlPanel.prototype.updateLogs = function(obj) {
    this.log('Dummy log.');
};

friendlyhal.ControlPanel.appLog = function(msg) {
    var listElem = goog.dom.createElement('li');
    goog.dom.setTextContent(listElem, msg);
   goog.dom.appendChild(goog.dom.getElement('applog'), listElem);
};

friendlyhal.ControlPanel.prototype.start = function() {
    // Fire off the XHRs;
    for (var url in this.urlToFunctionMap) {
	var updateFunction = this.urlToFunctionMap[url];
	goog.net.XhrIo.send(url, function(event) {
	    var xhrObj = event.target;
	    // Callback.
	    if (!xhrObj.isSuccess()) {
		friendlyhal.ControlPanel.appLog('XHR Error: ' + url + ' ' +
					        xhrObj.getStatusText());
	    } else {
		friendlyhal.ControlPanel.appLog('Loaded ' + url + '.');
		updateFunction(xhrObj.getResponseJson());
	    }
	});
    }
};