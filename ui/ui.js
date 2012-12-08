goog.provide('friendlyhal');

goog.require('goog');
goog.require('goog.dom');
goog.require('goog.net.XhrIo');

// Commands.
friendlyhal.PAN_LEFT = "/cgi-bin/action/panleft";
friendlyhal.PAN_RIGHT = "/cgi-bin/action/panright";
friendlyhal.PAN_UP = "/cgi-bin/action/panup";
friendlyhal.PAN_DOWN = "/cgi-bin/action/pandown";

friendlyhal.appLog = function(msg) {
    var listElem = goog.dom.createElement('li');
    goog.dom.setTextContent(listElem, msg);
    goog.dom.appendChild(goog.dom.getElement('applog'), listElem);
};

friendlyhal.start = function() {
};


friendlyhal.pan = function(panEvent) {
    goog.net.XhrIo.send(panEvent);
};

