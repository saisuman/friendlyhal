import event

class EventSource(object):
    SOURCE_NAME = 'UNDEFINED SOURCE'

    def __init__(self, callback_fn):
        raise Error('Cannot instantiate EventSource.')

    def start(self):
        raise Error('Unimplemented.')

    def stop(self):
        raise Error('Unimplemented.')

    def emit_start_event(self, message=None):
        self.callback_fn(Event(self.SOURCE_NAME, event.EVENT_START,
                               params={'message': message}))

    def emit_stop_event(self, message=None):
        self.callback_fn(Event(self.SOURCE_NAME, event.EVENT_STOP,
                               params={'message': message}))

    def emit_exception_event(self, message=None):
        self.callback_fn(Event(self.SOURCE_NAME, event.EVENT_EXCEPTION,
                               params={'message': message}))
