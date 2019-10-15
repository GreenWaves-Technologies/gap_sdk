# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers

class SplitVOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsSplitVOptions(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = SplitVOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def SplitVOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # SplitVOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # SplitVOptions
    def NumSplits(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Int32Flags, o + self._tab.Pos)
        return 0

def SplitVOptionsStart(builder): builder.StartObject(1)
def SplitVOptionsAddNumSplits(builder, numSplits): builder.PrependInt32Slot(0, numSplits, 0)
def SplitVOptionsEnd(builder): return builder.EndObject()
