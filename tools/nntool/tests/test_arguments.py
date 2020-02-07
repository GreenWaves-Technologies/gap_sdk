from graph.types import ReshapeParameters
from graph.dim import Dim

def test_reshape1():
    params = ReshapeParameters("test", old_shape=(8), shape=(2, 2, 2))
    out_size = params.get_output_size([Dim.unnamed([1, 8])])[0]
    assert out_size.shape == [2, 2, 2]
