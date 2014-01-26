import logging
import time

import data

SOURCE_TEMP_HUM_SENSOR = 'temp_hum_sensor'
SOURCE_PIR_SENSOR = 'pir_sensor'

EVENT_TYPE_DATA = 'data'
EVENT_TYPE_INIT = 'init'
EVENT_TYPE_ERROR = 'error'

DATA_TEMPERATURE = 'temperature'
DATA_HUMIDITY = 'humidity'
DATA_MOTION = 'motion'

COMMAND_PAN_UP = 'panup'
COMMAND_PAN_DOWN = 'pandown'


class Event(object):
    def __init__(self, event_type, event_source=None,
                 event_data=None):
        self.timestamp = int(time.time())
        self.event_type = event_type
        self.event_source = event_source
        self.event_data = event_data

    def __str__(self):
        return 'TIME:%s,TYPE=%s,SOURCE=%s,DATA=%s' % (
            self.timestamp, self.event_type, self.event_source,
            self.event_data)


class InitEvent(Event):
    def __init__(self, event_source):
        Event.__init__(self, event_type=EVENT_TYPE_INIT,
                       event_source=event_source)


class DataEvent(Event):
    def __init__(self, event_source, event_data):
        Event.__init__(self, event_type=EVENT_TYPE_DATA,
                       event_source=event_source,
                       event_data=event_data)

    def to_data_point(self):
        return data.DataPoint(timestamp=self.timestamp,
                              value=self.event_data)


class ErrorEvent(Event):
    def __init__(self, event_source):
        Event.__init__(self, event_type=EVENT_TYPE_ERROR,
                       event_source=event_source)


class MalformedEventException(Exception):
    def __init__(self, event_str):
        self.event_str = event_str

    def __str__(self):
        return 'Malformed event data: %s' % self.event_str


def to_event(event_str):
    sep_index = event_str.find(':')
    if sep_index == -1:
        raise MalformedEventException(event_str)
    event_type = event_str[0:sep_index]
    trailing_str = event_str[sep_index + 1:]
    event = None
    if event_type == EVENT_TYPE_ERROR:
        event = ErrorEvent(trailing_str)
    elif event_type == EVENT_TYPE_INIT:
        event = InitEvent(trailing_str)
    elif event_type == EVENT_TYPE_DATA:
        if trailing_str.find(',') == -1:
            raise MalformedEventException(event_str)
        source, data = trailing_str.split(',', 1)
        event = DataEvent(source, int(data))
    else:
        raise MalformedEventException(event_str)
    return event


if __name__ == '__main__':
    def test_malformed_event(event_str):
        try:
            e = to_event(event_str)
            raise Exception('Did not catch malformed event: %s' % event_str)
        except MalformedEventException as ex:
            pass

    print 'Running tests...'
    test_malformed_event('supercat1')
    test_malformed_event('data:temperature')
    test_malformed_event('error1:supercat')
    test_malformed_event('superdog:1')

    assert to_event('init:supercat').event_source == 'supercat'
    assert to_event('data:supercat,1').event_source == 'supercat'
    assert to_event('data:supercat,777').event_data == 777
    assert to_event('error:supercat').event_source == 'supercat'



