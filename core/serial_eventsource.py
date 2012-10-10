import serial
import threading
from core import eventsource

SERIAL_DEVICE = '/dev/ttyS0'
SERIAL_BAUD = 19200


class SerialEventSource(eventsource.EventSource):
    SOURCE_NAME = 'Serial Event Source'

    def __init__(self, callback_fn, serial_device=None):
        self.callback_fn_ = callback_fn
        self.serial_port_ = None
        self.stop_ = False

    def start(self):
        self.serial_port_ = serial.Serial(SERIAL_DEVICE, SERIAL_BAUD)
        self.emit_start_event(self.SOURCE_NAME)
        while True:
            self.lock_.acquire()
            try:
                if self.stop_:
                    return
                self.accumulate_and_emit_(self.serial_port_.readline())
            except Exception, e:
                self.stop_ = True
                self.emit_stop_event(str(e))
                break
            finally:
                self.lock_.release()

    def accumulate_and_emit_(self, line):
        if line.empty():  # PROTOCOL: Empty line is end-of-message
            if self.accumulated_data_.empty():
                return  # Nothing to publish
            else:
                self.emit(self.accumulated_data_)
                self.accumulated_data_ = ''
        else:
            self.accumulated_data_ += line + '\n'
            

    def stop(self):
        self.lock_.acquire()
        self.stop_ = True
        self.emit_stop_event()
        
