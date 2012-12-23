import serial
import threading
from core import eventsource

SERIAL_DEVICE = '/dev/ttyS0'
SERIAL_BAUD = 19200


class SerialEventSource(eventsource.EventSource):
    SOURCE_NAME = 'Serial Event Source'

    def __init__(self, callback_fn, serial_device=None):
        self._callback_fn = callback_fn
        self._serial_port = None
        self._stop = False
        self._accumulated_data = ''

    def start(self):
        self.serial_port_ = serial.Serial(SERIAL_DEVICE, SERIAL_BAUD)
        self.emit_start_event(self.SOURCE_NAME)
        while True:
            self.lock_.acquire()
            try:
                if self.stop_:
                    return
                self._accumulate_and_emit(self.serial_port_.readline())
            except Exception, e:
                self.stop_ = True
                self.emit_stop_event(str(e))
                break
            finally:
                self.lock_.release()

    def _accumulate_and_emit(self, line):
        if line.empty():  # PROTOCOL: Empty line is end-of-message
            if self._accumulated_data.empty():
                return  # Nothing to publish
            else:
                self.emit(self._accumulated_data)
                self._accumulated_data = ''
        else:
            self._accumulated_data += line + '\n'
            

    def stop(self):
        self.lock_.acquire()
        self.stop_ = True
        self.emit_stop_event()
