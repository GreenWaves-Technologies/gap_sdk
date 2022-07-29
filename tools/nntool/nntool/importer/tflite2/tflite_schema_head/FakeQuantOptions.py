# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class FakeQuantOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = FakeQuantOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsFakeQuantOptions(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    @classmethod
    def FakeQuantOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # FakeQuantOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # FakeQuantOptions
    def Min(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Float32Flags, o + self._tab.Pos)
        return 0.0

    # FakeQuantOptions
    def Max(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Float32Flags, o + self._tab.Pos)
        return 0.0

    # FakeQuantOptions
    def NumBits(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Int32Flags, o + self._tab.Pos)
        return 0

    # FakeQuantOptions
    def NarrowRange(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return False

def FakeQuantOptionsStart(builder): builder.StartObject(4)
def Start(builder):
    return FakeQuantOptionsStart(builder)
def FakeQuantOptionsAddMin(builder, min): builder.PrependFloat32Slot(0, min, 0.0)
def AddMin(builder, min):
    return FakeQuantOptionsAddMin(builder, min)
def FakeQuantOptionsAddMax(builder, max): builder.PrependFloat32Slot(1, max, 0.0)
def AddMax(builder, max):
    return FakeQuantOptionsAddMax(builder, max)
def FakeQuantOptionsAddNumBits(builder, numBits): builder.PrependInt32Slot(2, numBits, 0)
def AddNumBits(builder, numBits):
    return FakeQuantOptionsAddNumBits(builder, numBits)
def FakeQuantOptionsAddNarrowRange(builder, narrowRange): builder.PrependBoolSlot(3, narrowRange, 0)
def AddNarrowRange(builder, narrowRange):
    return FakeQuantOptionsAddNarrowRange(builder, narrowRange)
def FakeQuantOptionsEnd(builder): return builder.EndObject()
def End(builder):
    return FakeQuantOptionsEnd(builder)