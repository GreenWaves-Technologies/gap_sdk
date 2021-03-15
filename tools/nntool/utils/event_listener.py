class EventListenerStack():
    def __init__(self,  event):
        self.__event = event
        self.__listeners = []

    def respond(self, *data):
        if self.has_listeners():
            for listener in self.listeners:
                listener.respond(*data)
            return True
        return False

    def verify_has_listener(self, fn):
        return bool(self.extract_instance_of(fn))

    def attach_listener(self, fn, index):
        if not self.verify_has_listener(fn):
            self.__listeners.insert(index, EventListener(fn))

    def detach_listener(self, fn):
        listener = self.extract_instance_of(fn)
        if (listener):
            self.__listeners.remove(listener)
            return True
        else:
            return False

    def detach_all_listeners(self):
        for listener in self.listeners:
            self.detach_listener(listener)

    def has_listeners(self):
        return bool(self.listener_count)

    def extract_instance_of(self, fn):
        result = None
        for listener in self.listeners:
            if listener.verify(fn):
                result = listener
        return result

    @property
    def listeners(self):
        return list(self.__listeners)

    @property
    def listener_count(self):
        return len(self.listeners)


class EventListener():
    def __init__(self, listener):
        self._listener = listener
        self.__called_times = 0

    def respond(self, *data):
        self.__called_times += 1
        self._listener(*data)

    def verify(self, fn):
        return self._listener == fn

    @property
    def called_count(self):
        return self.__called_times


class EventEmitter:
    def __init__(self):
        self.__raw_listeners = {}

    def __onceWrap(self, event, listener):
        def wrapped_fn(*data):
            self.remove_listener(event, wrapped_fn)
            listener(*data)
        return wrapped_fn

    def __add_listener(self, event, listener, prepend):
        if not self.has_event(event):
            self.__listeners[event] = EventListenerStack(event)
        stack = self.get_stack_of(event)
        stack.attach_listener(listener, 0 if prepend else stack.listener_count)
        self.emit(f'addlistener:{event}')
        return self

    def on(self, event, listener):
        return self.__add_listener(event, listener, False)

    def prepend_listener(self, event, listener):
        return self.__add_listener(event, listener, True)

    def once(self, event, listener):
        return self.__add_listener(event, self.__onceWrap(event, listener), False)

    def prepend_once_listener(self, event, listener):
        return self.__add_listener(event, self.__onceWrap(event, listener), True)

    def emit(self, event, *data):
        return self.has_event(event) and self.get_stack_of(event).respond(*data)

    def add_listener(self, *args):
        return self.on(*args)

    def off(self, *args):
        return self.remove_listener(*args)

    def remove_listener(self, event, listener):
        if self.has_event(event, True):
            self.get_stack_of(event).detach_listener(listener)
            self.emit(f'rmlistener:{event}')
        return self

    def remove_all_listeners(self, event=None):
        if isinstance(event, str):
            if self.has_event(event, True):
                self.get_stack_of(event).detach_all_listeners()
            del self.__listeners[event]
            self.emit(f'rmlistener:{event}')
        else:
            for event in self.__listeners:
                self.remove_all_listeners(event)
        return self

    def has_event(self, event, raise_exception=False):
        status = event in self.__listeners and isinstance(self.__listeners[event], EventListenerStack)
        if not status and raise_exception:
            raise Exception(
                "Event: %s doesn't exist within EventEmitter instance" % event)
        return status

    def has_listeners(self, event):
        return self.has_event(event) and self.get_stack_of(event).has_listeners()

    def get_stack_of(self, event):
        return self.__listeners[event] if self.has_event(event) else None

    @property
    def __listeners(self):
        return self.__raw_listeners
