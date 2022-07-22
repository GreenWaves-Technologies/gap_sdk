import numpy as np

class QuantizedNdarray(np.ndarray):

    def __new__(cls, input_array, qtype=None, **kwargs):
        obj = np.asarray(input_array).view(cls)
        obj.qtype = qtype
        return obj

    def __array_finalize__(self, obj):
        if obj is None:
            return
#pylint: disable=attribute-defined-outside-init
        self.qtype = getattr(obj, 'qtype', None)

    def __array_ufunc__(self, ufunc, method, *inputs, **kwargs):  # this method is called whenever you use a ufunc
        '''this implementation of __array_ufunc__ makes sure that all custom attributes are maintained when a ufunc operation is performed on our class.
        It should really be a bit more descriminating since some ops will change the quantization'''

        # convert inputs and outputs of class ArraySubclass to np.ndarray to prevent infinite recursion
        args = ((i.view(np.ndarray) if isinstance(i, QuantizedNdarray) else i) for i in inputs)
        outputs = kwargs.pop('out', None)
        if outputs:
            kwargs['out'] = tuple((o.view(np.ndarray) if isinstance(o, QuantizedNdarray) else o) for o in outputs)
        else:
            outputs = (None,) * ufunc.nout
        # call numpys implementation of __array_ufunc__
        results = super().__array_ufunc__(ufunc, method, *args, **kwargs)  # pylint: disable=no-member
        if results is NotImplemented:
            return NotImplemented
        if method == 'at':
            # method == 'at' means that the operation is performed in-place. Therefore, we are done.
            return
        # now we need to make sure that outputs that where specified with the 'out' argument are handled corectly:
        if ufunc.nout == 1:
            results = (results,)
        results = tuple((self._copy_attrs_to(result) if output is None else output)
                        for result, output in zip(results, outputs))
        return results[0] if len(results) == 1 else results

    def _copy_attrs_to(self, target):
        '''copies all attributes of self to the target object. target must be a (subclass of) ndarray'''
        target = target.view(QuantizedNdarray)
        try:
            target.__dict__.update(self.__dict__)
        except AttributeError:
            pass
        return target