"""Fixed point conversion utilities.
From rig - https://github.com/project-rig/rig
"""
import numpy as np
# from .qtype import QType

def max_min(n_bits, signed):
    if signed:
        max_v = (1 << (n_bits - 1)) - 1
        min_v = -max_v - 1
    else:
        min_v = 0
        max_v = (1 << n_bits) - 1
    return min_v, max_v

def normalize(obj, n_bits):
    if n_bits == 0:
        return obj
    if n_bits < 0:
        return obj << -n_bits
    return obj >> n_bits


def clip(obj, qtype):
    min_v, max_v = max_min(qtype.bits, qtype.signed)
    ret = np.clip(obj, min_v, max_v).astype(qtype.dtype)
    return ret

def round_normalize(obj, cur_qtype, target_qtype):
    scale = cur_qtype.q - target_qtype.q
    obj = obj + (1<<(scale - 1))
    obj = normalize(obj, scale)
    return obj

def round_normalize_clip(obj, cur_qtype, target_qtype):
    scale = cur_qtype.q - target_qtype.q
    obj = obj + (1<<(scale - 1))
    obj = normalize(obj, scale)
    obj = clip(obj, target_qtype)
    return obj

def float_to_fp(signed, n_bits, n_frac):
    """Return a function to convert a floating point value to a fixed point
    value.

    For example, a function to convert a float to a signed fractional
    representation with 8 bits overall and 4 fractional bits (S3.4) can be
    constructed and used with::

        >>> s34 = float_to_fp(signed=True, n_bits=8, n_frac=4)
        >>> hex(int(s34(0.5)))
        '0x8'

    The fixed point conversion is saturating::

        >>> q34 = float_to_fp(False, 8, 4)  # Unsigned 4.4
        >>> hex(int(q34(-0.5)))
        '0x0'

        >>> hex(int(q34(15.0)))
        '0xf0'

        >>> hex(int(q34(16.0)))
        '0xff'

    Parameters
    ----------
    signed : bool
        Whether the values that are to be converted should be signed, or
        clipped at zero.

            >>> hex(int(float_to_fp(True, 8, 4)(-0.5)))  # Signed
            '-0x8'
            >>> hex(int(float_to_fp(False, 8, 4)(-0.5)))  # Unsigned
            '0x0'

    n_bits : int
        Total number of bits in the fixed-point representation (including sign
        bit and fractional bits).
    n_frac : int
        Number of fractional bits in the fixed-point representation.
    """
    # Calculate the maximum and minimum values
    if signed:
        max_v = (1 << (n_bits - 1)) - 1
        min_v = -max_v - 1
    else:
        min_v = 0
        max_v = (1 << n_bits) - 1

    # Compute the scale
    scale = 2.0**n_frac

    def bitsk(value):
        """Convert a floating point value to a fixed point value.

        Parameters
        ----------
        value : float
            The value to convert.
        """
        int_val = int(scale * value)
        return max((min(max_v, int_val), min_v))

    return bitsk


def fp_to_float(n_frac):
    """Return a function to convert a fixed point value to a floating point
    value.

    For example, a function to convert from signed fractional representations
    with 4 fractional bits constructed and used with::

        >>> f = fp_to_float(4)
        >>> f(0x08)
        0.5

        >>> f(-0x8)
        -0.5

        >>> f(-0x78)
        -7.5

    Parameters
    ----------
    n_frac : int
        Number of fractional bits in the fixed-point representation.
    """
    scale = 2.0**(-n_frac)

    def kbits(value):
        """Convert a fixed point value to a float.

        Parameters
        ----------
        value : int
            The fix point value as an integer.
        """
        return value * scale

    return kbits

class NumpyFloatToFixConverter:
    """A callable which converts Numpy arrays of floats to fixed point arrays.

    General usage is to create a new converter and then call this on arrays of
    values.  The `dtype` of the returned array is determined from the
    parameters passed.  For example::

        >>> f = NumpyFloatToFixConverter(signed=True, n_bits=8, n_frac=4)

    Will convert floating point values to 8-bit signed representations with 4
    fractional bits.  Consequently the returned `dtype` will be `int8`::

        >>> import numpy as np
        >>> vals = np.array([0.0, 0.25, 0.5, -0.5, -0.25])
        >>> f(vals)
        array([ 0,  4,  8, -8, -4], dtype=int8)

    The conversion is saturating::

        >>> f(np.array([15.0, 16.0, -16.0, -17.0]))
        array([ 127,  127, -128, -128], dtype=int8)

    The byte representation can be expected to match that for using
    `float_to_fix`::

        >>> d = f(np.array([-16.0]))

        >>> import struct
        >>> g = float_to_fix(True, 8, 4)
        >>> val = g(-16.0)
        >>> struct.pack('B', val) == bytes(d.data)
        True

    An exception is raised if the number of bits specified cannot be
    represented using a whole `dtype`::

        >>> NumpyFloatToFixConverter(True, 12, 0)
        Traceback (most recent call last):
        ValueError: n_bits: 12: Must be 8, 16, 32 or 64.
    """
    dtypes = {
        (False, 8): np.uint8,
        (True, 8): np.int8,
        (False, 16): np.uint16,
        (True, 16): np.int16,
        (False, 32): np.uint32,
        (True, 32): np.int32,
        (False, 64): np.uint64,
        (True, 64): np.int64,
    }

    def __init__(self, signed, n_bits, n_frac):
        """Create a new converter from floats into ints.

        Parameters
        ----------
        signed : bool
            Indicates that the converted values are to be signed or otherwise.
        n_bits : int
            The number of bits each value will use overall (must be 8, 16, 32,
            or 64).
        n_frac : int
            The number of fractional bits.
        """
        # Check the number of bits is sane
        if n_bits not in [8, 16, 32, 64]:
            raise ValueError(
                "n_bits: {}: Must be 8, 16, 32 or 64.".format(n_bits))

        # Determine the maximum and minimum values after conversion
        if signed:
            self.max_value = 2**(n_bits - 1) - 1
            self.min_value = -self.max_value - 1
        else:
            self.max_value = 2**n_bits - 1
            self.min_value = 0

        # Store the settings
        self.bytes_per_element = n_bits / 8
        self.dtype = self.dtypes[(signed, n_bits)]
        self.n_frac = n_frac

    def __call__(self, values):
        """Convert the given NumPy array of values into fixed point format."""
        # Scale and cast to appropriate int types
        vals = np.floor((values * 2.0 ** self.n_frac) + 0.5)

        # Saturate the values
        vals = np.clip(vals, self.min_value, self.max_value)

        # **NOTE** for some reason just casting resulted in shape
        # being zeroed on some indeterminate selection of OSes,
        # architectures, Python and Numpy versions"
        return np.array(vals, copy=True, dtype=self.dtype)


class NumpyFixToFloatConverter:
    """A callable which converts Numpy arrays of fixed point values to floating
    point.

    General usage is to create a new converter and then call this on arrays of
    values.  The `dtype` of the input array is used to determine whether the
    values are signed or otherwise.  For example, the following creates a
    callable which will convert from any format which has 4 fractional bits::

        >>> kbits = NumpyFixToFloatConverter(4)

    This will produced signed and unsigned values depending on the `dtype` of
    the original array.

        >>> signed = np.array([0xf0], dtype=np.int8)
        >>> kbits(signed)
        array([-1.])

        >>> unsigned = np.array([0xf0], dtype=np.uint8)
        >>> kbits(unsigned)[0]
        15.0
    """
    def __init__(self, n_frac):
        """Create a new converter from fix-point to floating point
        representation.

        Parameters
        ----------
        n_frac : int
            Number of fractional bits in the fixed-point representation.
        """
        self.n_frac = n_frac

    def __call__(self, values):
        """Convert the given NumPy array of values from fixed point format to
        floating point.

        ..note::
            The sign of the resulting values depends on the data type of the
            input values.  If the input is unsigned then the values will always
            be positive, otherwise the sign bit from the input value is used.
        """
        return values / (2.0**self.n_frac)

def validate_fp_params(signed, n_bits, n_frac):
    # Check the number of bits is sane
    if n_bits < 1:
        raise ValueError("n_bits: {}: Must be greater than 1.".format(n_bits))

    # Check the number of bits is possible
    signed_bit = 1 if signed else 0
    if signed_bit + n_frac > n_bits or n_frac < 0:
        raise ValueError(
            "n_frac: {}: Must be less than {} (and positive).".format(
                n_frac, n_bits - signed_bit)
        )

    # Account for the sign bit
    n_int = n_bits if not signed else n_bits - 1

    # Return the min and max values
    min_v = 0 if not signed else -(1 << (n_int - n_frac))
    max_v = ((1 << n_int) - 1) / float(1 << n_frac)

    return min_v, max_v
