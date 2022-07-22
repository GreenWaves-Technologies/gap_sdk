from typing import Any, Sequence

import numpy as np
from nntool.graph.types import NNNodeBase

from nntool.quantization.qtype import QType
from nntool.utils.json_serializable import JsonSerializable


class Cache(dict):
    def __setattr__(self, name: str, value: Any) -> None:
        assert not isinstance(value, np.ndarray)
        return super(Cache, self).__setattr__(name, value)


class QRec(JsonSerializable):
    def __init__(self, in_qs=None, out_qs=None, ktype=None,
                 auto_quantize_inputs=False, auto_dequantize_inputs=False,
                 auto_quantize_outputs=False, auto_dequantize_outputs=False,
                 **kwargs) -> None:
        self._in_qs = [] if in_qs is None else in_qs
        self._out_qs = [] if out_qs is None else out_qs
        if ktype is None:
            raise ValueError('ktype is not set')
        self._ktype = ktype
        self._val_cache = Cache(kwargs)
        self._auto_quantize_inputs = auto_quantize_inputs
        self._auto_dequantize_inputs = auto_dequantize_inputs
        self._auto_quantize_outputs = auto_quantize_outputs
        self._auto_dequantize_outputs = auto_dequantize_outputs

    def _encapsulate(self):
        return {
            k: getattr(self, k) for k in ['in_qs', 'out_qs', 'ktype']
        }.update(self._val_cache)

    @classmethod
    def _dencapsulate(cls, val):
        return QRec(**val)


    @classmethod
    def symmetric(cls, in_qs=None, out_qs=None,
                  auto_quantize_inputs=False, auto_dequantize_inputs=False,
                  auto_quantize_outputs=False, auto_dequantize_outputs=False,
                  **kwargs):
        return cls(in_qs=in_qs, out_qs=out_qs, ktype='symmetric',
                   auto_quantize_inputs=auto_quantize_inputs, auto_dequantize_inputs=auto_dequantize_inputs,
                   auto_quantize_outputs=auto_quantize_outputs, auto_dequantize_outputs=auto_dequantize_outputs,
                   **kwargs)

    @classmethod
    def scaled(cls, in_qs=None, out_qs=None,
               auto_quantize_inputs=False, auto_dequantize_inputs=False,
               auto_quantize_outputs=False, auto_dequantize_outputs=False,
               **kwargs):
        return cls(in_qs=in_qs, out_qs=out_qs, ktype='scaled',
                   auto_quantize_inputs=auto_quantize_inputs, auto_dequantize_inputs=auto_dequantize_inputs,
                   auto_quantize_outputs=auto_quantize_outputs, auto_dequantize_outputs=auto_dequantize_outputs,
                   **kwargs)

    @classmethod
    def float(cls, in_qs=None, out_qs=None,
              auto_quantize_inputs=False, auto_dequantize_inputs=False,
              auto_quantize_outputs=False, auto_dequantize_outputs=False,
              **kwargs):
        return cls(in_qs=in_qs, out_qs=out_qs, ktype='float',
                   auto_quantize_inputs=auto_quantize_inputs, auto_dequantize_inputs=auto_dequantize_inputs,
                   auto_quantize_outputs=auto_quantize_outputs, auto_dequantize_outputs=auto_dequantize_outputs,
                   **kwargs)

    @classmethod
    def copy_ktype(cls, qrec, in_qs=None, out_qs=None, **kwargs):
        return cls(in_qs=in_qs, out_qs=out_qs, ktype=qrec.ktype, **kwargs)

    @property
    def in_qs(self):
        return self._in_qs

    @in_qs.setter
    def in_qs(self, val):
        self._in_qs = val

    @property
    def out_qs(self):
        return self._out_qs

    @out_qs.setter
    def out_qs(self, val):
        self._out_qs = val

    @property
    def ktype(self):
        return self._ktype

    @ktype.setter
    def ktype(self, val):
        self._ktype = val

    @property
    def cache(self):
        return self._val_cache

    @property
    def auto_quantize_inputs(self):
        return self._auto_quantize_inputs

    @auto_quantize_inputs.setter
    def auto_quantize_inputs(self, val):
        self._auto_quantize_inputs = val

    @property
    def auto_dequantize_inputs(self):
        return self._auto_dequantize_inputs

    @auto_dequantize_inputs.setter
    def auto_dequantize_inputs(self, val):
        self._auto_dequantize_inputs = val

    @property
    def auto_quantize_outputs(self):
        return self._auto_quantize_outputs

    @auto_quantize_outputs.setter
    def auto_quantize_outputs(self, val):
        self._auto_quantize_outputs = val

    @property
    def auto_dequantize_outputs(self):
        return self._auto_dequantize_outputs

    @auto_dequantize_outputs.setter
    def auto_dequantize_outputs(self, val):
        self._auto_dequantize_outputs = val

    @staticmethod
    def qs_to_str(qtypes):
        if qtypes:
            return ",".join(str(qtype) if qtype else "None" for qtype in qtypes)
        return "None"

    @property
    def in_qs_str(self):
        self.qs_to_str(self._in_qs)

    @property
    def out_qs_str(self):
        self.qs_to_str(self._out_qs)

    def prepare_inputs(self,
                       params: NNNodeBase,
                       input_tensors: Sequence[np.ndarray],
                       ktype: str = None) -> Sequence[np.ndarray]:
        del params
        if ktype == 'float' and self.auto_dequantize_inputs:
            input_tensors = [in_q.dequantize(input_tensor)
                             for in_q, input_tensor in zip(self.in_qs, input_tensors)]
        elif ktype == "symmetric" and self._auto_quantize_inputs:
            return [self.in_qs[idx].quantize(t) if t is not None else None for idx, t in enumerate(input_tensors)]
        else:
            input_tensors = [
                t.copy() if t is not None else None for t in input_tensors]
        return input_tensors

    def get_outputs(self,
                    params: NNNodeBase,
                    output_tensors: Sequence[np.ndarray],
                    ktype: str = None) -> Sequence[np.ndarray]:
        del params
        if ktype == "symmetric":
            if self._auto_dequantize_outputs:
                return [self.out_qs[idx].dequantize(x) for idx, x in enumerate(output_tensors)]
            output_tensors = [self.out_qs[idx].clip(output_tensor)
                              for idx, output_tensor in enumerate(output_tensors)]
        return output_tensors


class AllFloatQType():
    def __getitem__(self, _):
        return QType(dtype=np.float32)


class AllFloatQRec(QRec):
    def __init__(self):
        super(AllFloatQRec, self).__init__(ktype='float')

    @property
    def in_qs(self):
        return AllFloatQType()

    @property
    def out_qs(self):
        return AllFloatQType()
