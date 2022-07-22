
class DumpRef():
    def __init__(self, ref_store, name) -> None:
        self._name = name
        self._ref_store = ref_store

    def __repr__(self) -> str:
        return f'{self._ref_store}[{repr(self._name)}]'

class DumpTensorRef(DumpRef):
    def __init__(self, name) -> None:
        super().__init__('tensors', name)

class DumpSubgraphRef(DumpRef):
    def __init__(self, name) -> None:
        super().__init__('subgraphs', name)

class DumpNodeRef(DumpRef):
    def __init__(self, name) -> None:
        super().__init__('nodes', name)
