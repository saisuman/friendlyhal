# Servlet for returning video streams in response
# to GET requests.
from mod_python import apache
from core import controller

def panup(req):
    controller.ControllerClient.command(controller.PANUP)
    return apache.OK

def pandown(req):
    controller.ControllerClient.command(controller.PANDOWN)
    return apache.OK
