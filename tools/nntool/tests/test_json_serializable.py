import json
from utils.json_serializable import JsonSerializable, JsonSerializableStateDecoder, JsonSerializableStateEncoder

class TestClass(JsonSerializable):
    def __init__(self, param):
        super(TestClass, self).__init__()
        self._param = param

    def _encapsulate(self):
        return self._param

    @classmethod
    def _dencapsulate(cls, val):
        return TestClass(val)

    def __eq__(self, value):
        return self._param == value._param

def test_json_serializer():
    val1 = TestClass("test")
    json_string = json.dumps(val1, cls=JsonSerializableStateEncoder)
    val2 = json.loads(json_string, cls=JsonSerializableStateDecoder)
    assert val1 == val2
