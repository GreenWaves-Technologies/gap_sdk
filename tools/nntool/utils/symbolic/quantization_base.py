from typing import Tuple
from utils.symbolic.basic import CompoundFunction

import numpy as np
from .symbol import Symbol, SymbolStats


class QRecBase():
    DTYPE_TO_CTYPE = {
        np.int8: 'int8_t',
        np.int16: 'int16_t',
        np.int32: 'int32_t',
        np.float32: 'float',
    }


class QuantizationHandlerBase():
    QHANDLERS = {}
    SCHEME = None

    def __init__(self, sym: Symbol) -> None:
        self._sym = sym

    @classmethod
    def handles_scheme(cls, scheme: str):
        return cls.property_register("SCHEME",
                                     scheme)

    @classmethod
    def qhandler(cls, scheme: str, *handles_clses):
        return cls.scheme_handler_property_register("QHANDLERS",
                                                    scheme,
                                                    *handles_clses)

    @staticmethod
    def property_register(name: str, val):

        def deco(cls):
            setattr(cls, name, val)
            return cls

        return deco

    @staticmethod
    def scheme_handler_property_register(name: str, scheme: str, *handles_clses):

        def deco(cls):
            handlers = getattr(cls, name)
            for handles_cls in handles_clses:
                cls_handlers = handlers.setdefault(handles_cls, {})
                cls_handlers[scheme] = cls
            return cls

        return deco

    @classmethod
    def get_handler(cls, scheme: str, sym: Symbol):
        return cls.QHANDLERS[sym.__class__][scheme]

    @classmethod
    def quantize_with_scheme(cls,
                             scheme: str,
                             sym: Symbol,
                             sym_ctrl: SymbolStats,
                             qrec: QRecBase = None,
                             **kwargs) -> Tuple[Symbol, QRecBase]:
        return cls.get_handler(scheme, sym).quantize(sym, sym_ctrl, qrec=qrec, **kwargs)

    @classmethod
    def _quantize(cls,
                  sym: Symbol,
                  sym_ctrl: SymbolStats,
                  qrec: QRecBase = None,
                  **kwargs) -> Tuple[Symbol, QRecBase]:
        """Overwridden in quantization class. Takes a symbol, stats on symbol ranges, and an optional forced qrec
        and returns the symbol wrapped with the appropriate quantization operations and the qrec giving its
        quantization"""
        raise NotImplementedError()

    @classmethod
    def _dequantize(cls, val, qrec: QRecBase, **kwargs) -> np.ndarray:
        """Overwridden in quantization class. Takes a value in qrec quantization and dequantizes it"""
        raise NotImplementedError()

    @classmethod
    def _dequantize_py_expr(cls, py_expr: str, qrec: QRecBase, **kwargs) -> str:
        """Overwridden in quantization class. Takes a python expression string in qrec quantization and generates the
        python code to dequantize it"""
        raise NotImplementedError()

    @classmethod
    def _dequantize_c_expr(cls, c_expr: str, qrec: QRecBase, **kwargs) -> str:
        """Overwridden in quantization class. Takes a c expression string in qrec quantization and generates the
        c code to dequantize it"""
        raise NotImplementedError()

    @classmethod
    def _quantize_output(cls,
                         sym: Symbol,
                         qsym: Symbol,
                         sym_ctrl: SymbolStats,
                         qrec: QRecBase,
                         **kwargs) -> Tuple[Symbol, QRecBase]:
        """Overwridden in quantization class. Takes a symbol in qrec quantization and does what is necessary to make
        it compatible with the quantization scheme's output format"""
        raise NotImplementedError()

    @classmethod
    def quantize_impl(cls,
                      sym: Symbol,
                      sym_ctrl: SymbolStats,
                      qrec: QRecBase = None,
                      **kwargs) -> Tuple[Symbol, QRecBase]:
        return cls._quantize(sym, sym_ctrl, qrec, **kwargs)

    @classmethod
    def quantize(cls,
                 sym: Symbol,
                 sym_ctrl: SymbolStats,
                 qrec: QRecBase = None,
                 **kwargs) -> Tuple[Symbol, QRecBase]:
        if isinstance(sym, CompoundFunction):
            qsym, qrec = cls.quantize(sym.resolve(), sym_ctrl, qrec=qrec, **kwargs)
        else:
            handler = cls.get_handler(cls.SCHEME, sym)
            qsym, qrec = handler.quantize_impl(sym, sym_ctrl, qrec, **kwargs)
        qsym.name = sym.name
        qsym.qrec = qrec
        return (qsym, qrec)

    @classmethod
    def dequantize(cls, val, qrec: QRecBase, **kwargs) -> np.ndarray:
        return cls._dequantize(val, qrec, **kwargs)

    @classmethod
    def quantize_output(cls,
                        sym: Symbol,
                        qsym: Symbol,
                        sym_ctrl: SymbolStats,
                        qrec: QRecBase,
                        **kwargs) -> Tuple[Symbol, QRecBase]:
        return cls._quantize_output(sym, qsym, sym_ctrl, qrec, **kwargs)


#pylint: disable=invalid-name
qhandler = QuantizationHandlerBase.qhandler
handles_scheme = QuantizationHandlerBase.handles_scheme
