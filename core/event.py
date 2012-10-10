import json
import time

SOURCE_NAME = 'System'  # For system level events.
SOURCE_NAME = 'Manual'  # For manually injected events.

EVENT_START = 'StartEvent'
EVENT_STOP = 'StopEvent'
EVENT_EXCEPTION = 'ExceptionEvent'
EVENT_DATA_POINT = 'DataPointEvent'

class Event(object):
    def __init__(self, source, event_type, timestamp=None,
                 params=None):
        self.source = source
        self.event_type = event_type
        self.timestamp = timestamp if timestamp else time.time()
        self.params = params if params else {}

    def to_json(self):
        json.dumps({
                'source': self.source,
                'event_type': self.event_type,
                'timestamp': self.timestamp,
                'params': self.params
                })

