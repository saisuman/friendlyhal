# Servlet for returning video streams in response
# to GET requests.
from mod_python import apache
from core import controller
from core import util

def send_single_command(cmd):
    c = controller.ControllerClient()
    c.send(cmd)
    c.close()

def send_recv_single_command(cmd):
    c = controller.ControllerClient()
    ret = c.send_recv(cmd)
    c.close()
    return ret


def panup(req):
    send_single_command(util.COMMAND_PAN_UP)
    return apache.OK

def pandown(req):
    send_single_command(util.COMMAND_PAN_DOWN)
    return apache.OK

def temperature(req):
    req.write(send_recv_single_command(util.DATA_TEMPERATURE))

def humidity(req):
    req.write(send_recv_single_command(util.DATA_HUMIDITY))

def motion(req):
    req.write(send_recv_single_command(util.DATA_MOTION))
