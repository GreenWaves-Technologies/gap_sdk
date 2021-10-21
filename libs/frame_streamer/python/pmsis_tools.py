#
# Copyright (C) 2019 GreenWaves Technologies
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

# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)

import ctypes
import numpy.ctypeslib
from numpy.ctypeslib import ndpointer
import numpy as np



pmsis_tools_module = None


def get_module():
    global pmsis_tools_module
    if pmsis_tools_module is None:
        pmsis_tools_module =  ctypes.CDLL("libpmsis_tools.so")

        pmsis_tools_module.transport_socket_open.argtypes = [ctypes.c_int]
        pmsis_tools_module.transport_socket_open.restype = ctypes.c_void_p

        pmsis_tools_module.frame_streamer_open.argtypes = [ctypes.c_void_p, ctypes.c_int]
        pmsis_tools_module.frame_streamer_open.restype = ctypes.c_void_p

        pmsis_tools_module.frame_streamer_get_stream.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
        pmsis_tools_module.frame_streamer_get_stream.restype = ctypes.c_void_p

        pmsis_tools_module.frame_stream_get_frame.argtypes = [ctypes.c_void_p, ndpointer(ctypes.c_ubyte, ndim=3, flags="C_CONTIGUOUS")]
        pmsis_tools_module.frame_stream_get_frame.restype = ctypes.POINTER(ctypes.c_char)

        pmsis_tools_module.pmsis_tools_init()

    return pmsis_tools_module


class Transport(object):

    def __init__(self):
        pass



class Transport_socket(Transport):

    def __init__(self, port):
        super(Transport_socket, self).__init__()
        self.instance = get_module().transport_socket_open(port)

    def get_instance(self):
        return self.instance



class Frame_stream(object):

    def __init__(self, instance, width, height, depth):
        self.instance = instance
        self.width = width
        self.height = height
        self.depth = depth

    def get_frames(self):
        while True:
            size = self.width * self.height * self.depth
            array = np.ndarray(shape=(self.width, self.height, self.depth), dtype='uint8')
            get_module().frame_stream_get_frame(self.instance, array)
            yield array



class Frame_streamer(object):

    def __init__(self, transport, show=True):
        self.transport = transport
        self.instance = get_module().frame_streamer_open(transport.get_instance(), show)

    def get_stream(self, name):
        width = ctypes.c_int(0)
        height = ctypes.c_int(0)
        depth = ctypes.c_int(0)

        stream = get_module().frame_streamer_get_stream(self.instance, name.encode('utf-8'), ctypes.byref(width), ctypes.byref(height), ctypes.byref(depth))

        return Frame_stream(stream, width.value, height.value, depth.value)
