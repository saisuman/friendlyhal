import event

class VoiceCommandEventSource(object):
    """Provides a stream of voice commands.

    The voice commands are actually read off of a process
    that runs a PocketSphinx processor trained on a small
    vocabulary of commands.
    """
    SOURCE_NAME = 'VoiceCommandSource'

    def __init__(self, callback_fn):
        self.callback_fn = callback_fn

    def start(self):
        self.emit_start_event(self.SOURCE_NAME)
        while True:
            self.lock_.acquire()
            try:
                if self.stop_:
                    return
            except Exception, e:
                self.stop_ = True
                self.emit_stop_event(str(e))
                break
            finally:
                self.lock_.release()

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
