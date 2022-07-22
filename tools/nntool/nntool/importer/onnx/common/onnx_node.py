from nntool.importer.onnx import common


class OnnxNode(object):
    """
    Reimplementation of NodeProto from ONNX, but in a form
    more convenient to work with from Python.
     """

    def __init__(self, node):
        self.name = str(node.name)
        self.op_type = str(node.op_type)
        self.domain = str(node.domain)
        self.attrs = dict([(attr.name,
                            common.translate_onnx(
                                attr.name, common.convert_onnx(attr)))
                           for attr in node.attribute])
        self.input = list(node.input)
        self.output = list(node.output)
        self.node_proto = node
