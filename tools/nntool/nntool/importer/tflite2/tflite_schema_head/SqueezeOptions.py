# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class SqueezeOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = SqueezeOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsSqueezeOptions(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    @classmethod
    def SqueezeOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # SqueezeOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # SqueezeOptions
    def SqueezeDims(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            a = self._tab.Vector(o)
            return self._tab.Get(flatbuffers.number_types.Int32Flags, a + flatbuffers.number_types.UOffsetTFlags.py_type(j * 4))
        return 0

    # SqueezeOptions
    def SqueezeDimsAsNumpy(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.GetVectorAsNumpy(flatbuffers.number_types.Int32Flags, o)
        return 0

    # SqueezeOptions
    def SqueezeDimsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # SqueezeOptions
    def SqueezeDimsIsNone(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        return o == 0

def SqueezeOptionsStart(builder): builder.StartObject(1)
def Start(builder):
    return SqueezeOptionsStart(builder)
def SqueezeOptionsAddSqueezeDims(builder, squeezeDims): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(squeezeDims), 0)
def AddSqueezeDims(builder, squeezeDims):
    return SqueezeOptionsAddSqueezeDims(builder, squeezeDims)
def SqueezeOptionsStartSqueezeDimsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def StartSqueezeDimsVector(builder, numElems):
    return SqueezeOptionsStartSqueezeDimsVector(builder, numElems)
def SqueezeOptionsEnd(builder): return builder.EndObject()
def End(builder):
    return SqueezeOptionsEnd(builder)