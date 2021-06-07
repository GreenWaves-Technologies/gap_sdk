#
# Copyright (C) 2021 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import socket
import threading
import socket




class Proxy(object):
    """
    A class used to control GVSOC through the socket proxy

    Attributes
    ----------
    host : str
        a string giving the hostname where the proxy is running
    port : int
        the port where to connect
    """

    class _Socket_proxy_reader_thread(threading.Thread):

        def __init__(self, socket):
            super(Proxy._Socket_proxy_reader_thread, self).__init__()
            self.socket = socket
            self.lock = threading.Lock()
            self.condition = threading.Condition(self.lock)
            self.replies = []
            self.matches = {}

        def run(self):
            while True:
                reply = ""
                try:
                    while True:
                        byte = self.socket.recv(1).decode('utf-8')
                        reply += byte
                        if byte == '\n':
                            break
                except:
                    return

                callback = self.matches.get(reply)
                if callback is not None:
                    callback[0](*callback[1], **callback[2])
                else:
                    self.lock.acquire()
                    self.replies.append(reply)
                    self.condition.notify()
                    self.lock.release()


        def wait_reply(self):
            self.lock.acquire()
            while len(self.replies) == 0:
                self.condition.wait()
            reply = self.replies.pop(0)

            self.lock.release()

            return reply

        def register_callback(self, match, callback, *kargs, **kwargs):
            self.matches[match] = callback, kargs, kwargs

        def unregister_callback(self, match):
            self.matches[match] = None


    def __init__(self, host: str = 'localhost', port: int = 42951):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((host, port))

        self.reader = self._Socket_proxy_reader_thread(self.socket)
        self.reader.start()


    def close(self):
        """Close the proxy.

        This will free resources and close threads so that simulation can properly exit.

        """
        self.socket.shutdown(socket.SHUT_WR)
        self.socket.close()
        self.reader.join()


    def trace_add(self, trace: str):
        """Enable a trace.

        Parameters
        ----------
        trace : str
            A regular expression used to enable traces
        """

        self.socket.send(('trace add %s\n' % trace).encode('ascii'))

    def trace_remove(self, trace: str):
        """Disable a trace.

        Parameters
        ----------
        trace : str
            A regular expression used to disable traces
        """

        self.socket.send(('trace remove %s\n' % trace).encode('ascii'))

    def trace_level(self, level: str):
        """Changes the trace level.

        Parameters
        ----------
        level : str
            The trace level, can be "error", "warning", "info", "debug" or "trace"
        """

        self.socket.send(('trace level %s\n' % level).encode('ascii'))

    def event_add(self, event: str):
        """Enable an event.

        Parameters
        ----------
        event : str
            A regular expression used to enable events
        """

        self.socket.send(('event add %s\n' % event).encode('ascii'))

    def event_remove(self, event: str):
        """Disable a trace.

        Parameters
        ----------
        event : str
            A regular expression used to enable events
        """

        self.socket.send(('event remove %s\n' % event).encode('ascii'))

    def run(self, duration: int = None):
        """Starts execution.

        Parameters
        ----------
        duration : int, optional
            Specify the duration of the execution in picoseconds (will execute forever by default)
        """

        if duration is not None:
            self.socket.send(('step %d\n' % duration).encode('ascii'))
        else:
            self.socket.send('run\n'.encode('ascii'))

        self.reader.wait_reply()

    def quit(self, status: int = 0):
        """Exit simulation.

        Parameters
        ----------
        duration : int, optional
            Specify the status value.
        """

        self.socket.send(('quit %d\n' % status).encode('ascii'))


    def _handle_err(self, error, error_str=None):
        if error != 0:
            if error_str is None:
                raise RuntimeError("Proxy command failed with status %s" % error)
            else:
                raise RuntimeError("Proxy command failed with message: %s" % error_str)


    def _get_retval(self):
        result = self.reader.wait_reply()
        error = 0
        error_str = None
        for arg in result.split(';'):
            name, value = arg.split('=')
            if name == 'err':
                error = int(value)
            elif name == 'msg':
                error_str = value

        self._handle_err(error, error_str)

        if error != 0:
            if error_str is None:
                raise RuntimeError("Proxy command failed with status %s" % error)
            else:
                raise RuntimeError("Proxy command failed with message: %s" % error_str)


    def _get_component(self, path):
            self.socket.send(('get_component %s\n' % path).encode('utf-8'))
            result = self.reader.wait_reply()
            return result.replace('\n', '')


class Router(object):
    """
    A class used to inject memory accesses into a router

    Attributes
    ----------
    proxy : Proxy
        The proxy object. This class will use it to send command to GVSOC through the proxy connection.
    path : string, optional
        The path to the router in the architecture.
    """

    def __init__(self, proxy: Proxy, path: str = '/sys/board/chip/soc/axi_ico'):
        self.proxy = proxy
        self.lock = threading.Lock()
        self.condition = threading.Condition(self.lock)
        self.pending_read_bytes = []
        self.component = proxy._get_component(path)
        self.proxy.reader.register_callback('router %s read\n' % self.component, self.__handle_read)


    def __handle_read(self):

        self.lock.acquire()

        size = self.read_size
        reply = []
        while size > 0:
            reply += self.proxy.socket.recv(size)
            size = self.read_size - len(reply)
        
        self.pending_read_bytes = reply
        self.condition.notify()
        self.lock.release()


    def mem_write(self, addr: int, size: int, values: bytes):
        """Inject a memory write.

        The access is generated by the router where this class is connected and is
        injected as a debug request to not disturb the timing.

        Parameters
        ----------
        addr : int
            The address of the access.
        size : int
            The size of the access in bytes.
        values : bytes
            The sequence of bytes to be written, in little endian byte ordering.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        cmd = 'component %s mem_write 0x%x 0x%x\n' % (self.component, addr, size)

        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy.socket.send(values)

        self.proxy._get_retval()

    def mem_read(self, addr: int, size: int) -> bytes:
        """Inject a memory read.

        The access is generated by the router where this class is connected and is
        injected as a debug request to not disturb the timing.

        Parameters
        ----------
        addr : int
            The address of the access.
        size : int
            The size of the access in bytes.

        Returns
        -------
        bytes
            The sequence of bytes read, in little endian byte ordering.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """

        self.read_size = size
        cmd = 'component %s mem_read 0x%x 0x%x\n' % (self.component, addr, size)
        self.proxy.socket.send(cmd.encode('ascii'))

        self.lock.acquire()
        
        while len(self.pending_read_bytes) < size:
            self.condition.wait()

        reply = self.pending_read_bytes
        self.pending_read_bytes = []

        self.lock.release()

        self.proxy._get_retval()

        return reply


    def mem_write_int(self, addr: int, size: int, value: int):
        """Write an integer.

        The access is generated by the router where this class is connected and is
        injected as a debug request to not disturb the timing.

        Parameters
        ----------
        addr : int
            The address of the access.
        size : int
            The size of the access in bytes.
        value : int
            The integer to be written.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        return self.mem_write(addr, size, value.to_bytes(size, byteorder='little'))

    def mem_read_int(self, addr: int, size: int) -> int:
        """Read an integer.

        The access is generated by the router where this class is connected and is
        injected as a debug request to not disturb the timing.

        Parameters
        ----------
        addr : int
            The address of the access.
        size : int
            The size of the access in bytes.

        Returns
        -------
        int
            The integer read.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        values = self.mem_read(addr, size)
        return int.from_bytes(values, byteorder='little')




class Testbench(object):
    """Testbench class.

    This class can be instantiated to get access to the testbench.

    Attributes
    ----------
    proxy : Proxy
        The proxy object. This class will use it to send command to GVSOC through the proxy connection.
    path : string, optional
        The path to the testbench in the architecture.
    """

    def __init__(self, proxy: Proxy, path: str = '/sys/board/testbench/testbench'):
        self.proxy = proxy
        self.component = proxy._get_component(path)


    def i2s_get(self, id: int = 0):
        """Open an SAI.

        Opena an SAI and return an object which can be used to interact with it.

        Parameters
        ----------
        id : int, optional
            The SAI identifier.

        Returns:
        Testbench_i2s
            An object which can be used to access the specified SAI.
        """
        return Testbench_i2s(self.proxy, self.component, id)


    def uart_get(self, id: int = 0):
        """Open a uart interface.

        Opena a uart interface and return an object which can be used to interact with it.

        Parameters
        ----------
        id : int, optional
            The uart interface identifier.

        Returns:
        Testbench_uart
            An object which can be used to access the specified uart interface.
        """
        return Testbench_uart(self.proxy, self.component, id)


class Testbench_uart(object):
    """Class instantiated for each manipulated uart interface.

    It can used to interact with the uart interface, like injecting streams.

    Attributes
    ----------
        proxy : Proxy
            The proxy object. This class will use it to send command to GVSOC through the proxy connection.
        testbench : int
            The testbench object.
        id : int, optional
            The identifier of the uart interface.
    """

    def __init__(self, proxy: Proxy, testbench: Testbench, id=0):
        self.id = id
        self.proxy = proxy
        self.testbench = testbench
        self.callback = None
        self.lock = threading.Lock()
        self.condition = threading.Condition(self.lock)
        self.pending_rx_bytes = []

    def open(self, baudrate: int, word_size: int=8, stop_bits: int=1, parity_mode: bool=False, ctrl_flow: bool=True,
            is_usart: bool=False, usart_polarity: int=0, usart_phase: int=0):
        """Open and configure a uart interface.

        Parameters
        ----------
        baudrate : int
            Specify the uart baudrate in bps
        word_size : int, optional
            Specify the size in bits of the uart bytes.
        stop_bits : int, optional
            Specify the number of stop bits.
        parity_mode : bool, optional
            True if parity is enabled.
        ctrl_flow : bool, optional
            True if control flow is enabled.
        is_usart : bool, optional
            True if uart is in usart mode.
        usart_polarity: int, optional
            Usart polarity.
        usart_phase: int, optional
            Usart phase.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """

        options = ''
        options += ' itf=%d' % self.id
        options += ' enabled=1'
        options += ' baudrate=%d' % baudrate
        options += ' word_size=%d' % word_size
        options += ' stop_bits=%d' % stop_bits
        options += ' parity_mode=%d' % parity_mode
        options += ' ctrl_flow=%d' % ctrl_flow
        options += ' is_usart=%d' % is_usart
        options += ' usart_polarity=%d' % usart_polarity
        options += ' usart_phase=%d' % usart_phase
        cmd = 'component %s uart setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def close(self):
        """Close the uart interface.
    
        Raises
        ------
        RuntimeError
            If there is any error while closing.
        """
        options = ''
        options += ' itf=%d' % self.id
        options += ' enabled=0'
        cmd = 'component %s uart setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def tx(self, values: bytes):
        """Send data to the uart.

        This enqueues an array of bytes to be transmitted. If previous transfers are not finished,
        these bytes will be transfered after.

        Parameters
        ----------
        values : bytes
            The sequence of bytes to be sent, in little endian byte ordering.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        cmd = 'component %s uart tx %d %d\n' % (self.testbench, self.id, len(values))

        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy.socket.send(values)

        self.proxy._get_retval()

    def rx(self, size=None):
        """Read data from the uart.

        Once reception on the uart is enabled, the received bytes are pushed to a fifo. This method
        can be called to pop received bytes from the FIFO.

        Parameters
        ----------
        size : int
            The number of bytes to be read. If it is None, it returns the bytes which has already been received.

        Returns
        -------
        bytes
            The sequence of bytes received, in little endian byte ordering.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        self.lock.acquire()

        if size is not None:
            while len(self.pending_rx_bytes) < size:
                self.condition.wait()
        else:
            size = len(self.pending_rx_bytes)

        reply = self.pending_rx_bytes[0:size]
        self.pending_rx_bytes = self.pending_rx_bytes[size:]

        self.lock.release()

        return reply


    def rx_enable(self):
        """Enable receiving bytes from the uart.

        Any byte received from the uart either triggers the callback execution if it has been registered,
        or is pushed to a FIFO which can read.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        self.proxy.reader.register_callback('uart rx %d\n' % self.id, self.__handle_rx)
        cmd = 'component %s uart rx %d 1\n' % (self.testbench, self.id)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()


    def rx_disable(self):
        """Dsiable receiving bytes from the uart.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        self.proxy.reader.unregister_callback('uart rx %d\n' % self.id)
        cmd = 'component %s uart rx %d 0\n' % (self.testbench, self.id)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()


    def __handle_rx(self):
        self.lock.acquire()
        reply = self.proxy.socket.recv(1)
        if self.callback is not None:
            self.callback[0](1, reply, *self.callback[1], **self.callback[2])
        else:
            self.pending_rx_bytes += reply
            self.condition.notify()
        self.lock.release()


    def rx_attach_callback(self, callback, *kargs, **kwargs):
        """Attach callback for receiving bytes from the uart.

        All bytes received from the uart now triggers the execution of the specified callback.
        This must be called only when uart reception is disabled.
        The callback will be called asynchronously by a different thread and so special care must be taken
        to access shared variables using locks. Also the proxy can not be used from the callback.

        Parameters
        ----------
        callback
            The function to be called when bytes are received from the uart. First parameters will contain
            number of bytes and received, and second one will be the bytes received.
        kargs, kwargs
            Arguments propagated to the callback

        Returns
        -------
        bytes
            The sequence of bytes received, in little endian byte ordering.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        self.callback = callback, kargs, kwargs


    def rx_detach_callback(self):
        """Detach a callback.

        The callback previously attached won't be called anymore.
        This must be called only when uart reception is disabled.

        Raises
        ------
        RuntimeError
            If the access generates an error in the architecture.
        """
        self.callback = None


class Testbench_i2s(object):
    """Class instantiated for each manipulated SAI.

    It can used to interact with the SAI, like injecting streams.

    Attributes
    ----------
        proxy : Proxy
            The proxy object. This class will use it to send command to GVSOC through the proxy connection.
        testbench : int
            The testbench object.
        id : int, optional
            The identifier of the SAI interface.
    """

    def __init__(self, proxy: Proxy, testbench: Testbench, id=0):
        self.id = id
        self.proxy = proxy
        self.testbench = testbench

    def open(self, word_size: int = 16, sampling_freq: int = -1, nb_slots: int = 1, is_pdm: bool = False,
            is_full_duplex: bool = False, is_ext_clk: bool = False, is_ext_ws: bool = False, is_sai0_clk: bool = False,
            is_sai0_ws: bool = False, clk_polarity: int = 0, ws_polarity: int = 0):
        """Open and configure SAI.

        Parameters
        ----------
        word_size : int, optional
            Specify the frame word size in bits.
        sampling_freq : int, optional
            Specify the sampling frequency. This is used either to generate the clock when
            it is external or to check that internally generated one is correct.
        nb_slots : int, optional
            Number of slots in the frame.
        is_pdm : bool, optional
            True if the stream is a PDM stream.
        is_full_duplex : bool, optional
            True if the SAI is used in full duplex mode.
        is_ext_clk: bool, optional
            True is the clock is generated by the testbench.
        is_ext_ws: bool, optional
            True is the word strobe is generated by the testbench.
        is_sai0_clk: bool, optional
            True is the the clock should be taken from SAI0.
        is_sai0_ws: bool, optional
            True is the the word strobe should be taken from SAI0.
        clk_polarity : int, optional
            Clock polarity, definition is the same as SAI0 specifications.
        ws_polarity : int, optional
            Word strobe polarity, definition is the same as SAI0 specifications.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """

        options = ''
        options += ' itf=%d' % self.id
        options += ' enabled=1'
        options += ' sampling_freq=%d' % sampling_freq
        options += ' word_size=%d' % word_size
        options += ' nb_slots=%d' % nb_slots
        options += ' is_pdm=%d' % is_pdm
        options += ' is_full_duplex=%d' % is_full_duplex
        options += ' is_ext_clk=%d' % is_ext_clk
        options += ' is_ext_ws=%d' % is_ext_ws
        options += ' is_sai0_clk=%d' % is_sai0_clk
        options += ' is_sai0_ws=%d' % is_sai0_ws
        options += ' clk_polarity=%d' % clk_polarity
        options += ' ws_polarity=%d' % ws_polarity
        cmd = 'component %s i2s setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def close(self):
        """Close SAI.
    
        Raises
        ------
        RuntimeError
            If there is any error while closing.
        """
        options = ''
        options += ' itf=%d' % self.id
        options += ' enabled=0'
        cmd = 'component %s i2s setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def clk_start(self):
        """Start clock.

        This can be used when the clock is generated by the testbench to start the generation.

        Raises
        ------
        RuntimeError
            If there is any error while starting the clock.
        """
        cmd = 'component %s i2s clk_start %d\n' % (self.testbench, self.id)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def clk_stop(self):
        """Stop clock.

        This can be used when the clock is generated by the testbench to stop the generation.

        Raises
        ------
        RuntimeError
            If there is any error while stopping the clock.
        """
        cmd = 'component %s i2s clk_stop %d\n' % (self.testbench, self.id)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def slot_open(self, slot: int = 0, is_rx: bool = True, word_size: int = 16, is_msb: bool = True,
            sign_extend: bool = False, left_align: bool = False):
        """Open and configure a slot.

        Parameters
        ----------
        slot : int, optional
            Slot identifier
        is_rx : bool, optional
            True if gap receives the samples.
        word_size : int, optional
            Slot width in number of bits.
        is_msb : bool, optional
            True if the samples are received or sent with MSB first.
        sign_extend : bool, optional
            True if the samples are sign-extended.
        left_align : bool, optional
            True if the samples are left aligned.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """
        options = ''
        options += ' itf=%d' % self.id
        options += ' slot=%d' % slot
        options += ' is_rx=%d' % is_rx
        options += ' enabled=1'
        options += ' word_size=%d' % word_size
        options += ' format=%d' % (is_msb | (left_align << 1) | (sign_extend << 1))
        cmd = 'component %s i2s slot_setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def slot_close(self, slot: int = 0):
        """Close a slot.

        Parameters
        ----------
        slot : int, optional
            Slot identifier

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """
        options = ''
        options += ' itf=%d' % self.id
        options += ' slot=%d' % slot
        options += ' enabled=0'
        cmd = 'component %s i2s slot_setup %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def slot_rx_file_reader(self, slot: int = None, slots: list = [], filetype: str = "wav", filepath: str = None, channel: int = 0):
        """Read a stream of samples from a file.

        This will open a file and stream it to the SAI so that gap receives the samples.
        It can be used either in mono-channel mode with the slot parameter or multi-channel mode with the
        slots parameter. In multi-channel mode, the slots parameters give the list of slots associated to each channel.
        To allow empty channels, a slot of -1 can be given.

        Parameters
        ----------
        slot : int, optional
            Slot identifier
        slots : list, optional
            List of slots when using multi-channel mode. slot must be None if this one is not empty.
        filetype : string, optional
            Describes the type of the file, can be "wav" or "au".
        filepath : string, optional
            Path to the file.
        channel : int, optional
            If the format supports it, this will get the samples from the specified channel in the input file.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """
        options = ''
        options += ' itf=%d' % self.id
        if slot is not None:
            options += ' slot=%d' % slot
        for slot in slots:
            options += ' slot=%d' % slot
        options += ' filetype=%s' % filetype
        options += ' filepath=%s' % filepath
        options += ' channel=%d' % channel
        cmd = 'component %s i2s slot_rx_file_reader %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def slot_tx_file_dumper(self, slot: int = None, slots: list = [], filetype: str = "wav", filepath: str = None, channel: int = 0):
        """Write a stream of samples to a file.

        This will open a file and write to it all the samples received from gap.
        It can be used either in mono-channel mode with the slot parameter or multi-channel mode with the
        slots parameter. In multi-channel mode, the slots parameters give the list of slots associated to each channel.
        To allow empty channels, a slot of -1 can be given. A slot can be given several times in order to push the samples
        to several channels.

        Parameters
        ----------
        slot : int, optional
            Slot identifier
        slots : list, optional
            List of slots when using multi-channel mode. slot must be None if this one is not empty.
        filetype : string, optional
            Describes the type of the file, can be "wav" or "au".
        filepath : string, optional
            Path to the file.
        channel : int, optional
            If the format supports it, this will dump the samples to the specified channel in the output file.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """
        options = ''
        options += ' itf=%d' % self.id
        if slot is not None:
            options += ' slot=%d' % slot
        for slot in slots:
            options += ' slot=%d' % slot
        options += ' filetype=%s' % filetype
        options += ' filepath=%s' % filepath
        options += ' channel=%d' % channel
        cmd = 'component %s i2s slot_tx_file_dumper %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()

    def slot_stop(self, slot: int = 0, stop_rx: bool = True, stop_tx: bool = True):
        """Stop a slot.

        This will stop the streamings (file reader or dumper) configured on the specified slot.

        Parameters
        ----------
        slot : int, optional
            Slot identifier
        stop_rx : bool, optional
            Stop the stream sent to gap.
        stop_tx : bool, optional
            Stop the stream received from gap.

        Raises
        ------
        RuntimeError
            If there is any invalid parameter.
        """
        options = ''
        options += ' itf=%d' % self.id
        options += ' slot=%d' % slot
        options += ' stop_rx=%d' % stop_rx
        options += ' stop_tx=%d' % stop_tx
        cmd = 'component %s i2s slot_stop %s\n' % (self.testbench, options)
        self.proxy.socket.send(cmd.encode('ascii'))
        self.proxy._get_retval()
