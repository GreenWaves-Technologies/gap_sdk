from utils.data_importer import import_data

def test_import_png(ir_images):
    data = import_data(ir_images[0], offset=-1, divisor=128)
    assert data.max() < 1 and data.min() > -1
    assert data.shape == (1, 80, 80)
