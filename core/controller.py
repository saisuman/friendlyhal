class MicroController(object):
    COMMAND_PAN_LEFT = 'panleft'
    COMMAND_PAN_RIGHT = 'panright'
    COMMAND_PAN_UP = 'panup'
    COMMAND_PAN_DOWN = 'pandown'

    def __init__(self, device='/dev/ttyS0', baud=9600):
        self.serial_port_ = serial.Serial(device, baud)

    def send(command):
        self.serial_port_.write(command)
