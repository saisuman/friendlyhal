#!/usr/bin/python
import serial
import socket
import os
import threading
import json

import data
import util

LISTENER_SOCKET = '/tmp/controller_listener.socket'

class ListenerThread(threading.Thread):
    def __init__(self, serial_port):
        threading.Thread.__init__(self)
        self.serial_port = serial_port
        self.data_timeseries = {
            util.DATA_TEMPERATURE: data.TimeSeries(),
            util.DATA_HUMIDITY: data.TimeSeries(),
            util.DATA_MOTION: data.TimeSeries(),
            }

    def run(self):
        while True:
            line = self.serial_port.readline()
            try:
                event = util.to_event(line.strip())
                if isinstance(event, util.DataEvent):
                    self.data_timeseries[
                        event.event_source].add(event.to_data_point())
                print 'OK: %s' % event
            except util.MalformedEventException as ex:
                print 'ERROR: %s' % ex


class ControllerListener(object):
    def __init__(self, socketdev=LISTENER_SOCKET,
                 device='/dev/ttyACM0', baud=9600):
        try:
            os.remove(socketdev)
        except OSError:
            pass
        self.serial_port = serial.Serial(device, baud)
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.bind(socketdev)
        os.chmod(socketdev, 0777)
	print 'Listened'
        self.listener = ListenerThread(self.serial_port)
        self.listener.start()

    def maybe_handle_data_request(self, data, conn):
        print 'OK: Checking if %s is a command.' % data
        if data.strip() in self.listener.data_timeseries:
            conn.send(json.dumps(
                    self.listener.data_timeseries[data.strip()].to_array()))
            conn.close()
            return True
        return False

    def start(self):
        self.socket.listen(10)
        while True:
            try:
                conn, addr = self.socket.accept()
                data = conn.recv(1024)
                if self.maybe_handle_data_request(data, conn):
                    continue
                # If it's not a data request, we simply
                # forward it to the serial port.
                self.serial_port.write(data)
                self.serial_port.write('\r')
                self.serial_port.flush()
                print('Relay: %s' % data)
            except Exception as e:
                print('ERROR: Raised exception: %s' % e)


class ControllerClient(object):
    def __init__(self):
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.connect(LISTENER_SOCKET)

    def send(self, cmd):
        self.socket.send(cmd)

    def send_recv(self, cmd):
        self.send(cmd)
        return self.socket.recv(1048576)

    def close(self):
        self.socket.close()


def main():
    ControllerListener().start()


if __name__ == '__main__':
    main()
