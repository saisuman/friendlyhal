import socket
import os

LISTENER_SOCKET = '/tmp/controller_listener.socket'

COMMAND_PAN_UP = 'panup'
COMMAND_PAN_DOWN = 'pandown'


class ControllerListener(object):
    def __init__(self, socketdev=LISTENER_SOCKET):
        try:
            os.remove(socketdev)
        except OSError:
            pass
        self.socket_ = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket_.bind(socketdev)
	print 'Listened'

    def start(self):
        self.socket_.listen(10)
        while True:
            conn, addr = self.socket_.accept()
            data = conn.receive(1024)
            print 'Received data: %s' % data
            conn.close()
     
def main():
    ControllerListener().start()
	


def ControllerClient(object):
    @classmethod
    def command(cmd):
        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        s.connect(LISTENER_SOCKET)
        s.send(cmd)
        s.close()


class MicroController(object):
    def __init__(self, device='/dev/ttyS0', baud=9600):
        self.serial_port_ = serial.Serial(device, baud)

    def send(command):
        self.serial_port_.write(command)


if __name__ == '__main__':
	main()
