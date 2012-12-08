# Servlet for returning video streams in response
# to GET requests.
from mod_python import apache
from core import controller

def panleft(req):
    controller.pan_left()
    return apache.OK

def panright(req):
    controller.pan_right()
    return apache.OK

def panup(req):
    controller.pan_up()
    return apache.OK

def pandown(req):
    controller.pan_down()
    return apache.OK
