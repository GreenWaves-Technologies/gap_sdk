# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

class NodeId():
    def __init__(self, node, fnode=None):
        if isinstance(node, list):
            self._id = [None if not elem else elem for elem in node]
        else:
            self._id = [node.name, "" if fnode is None else fnode.name]

    @property
    def id(self):
        return self._id

    @property
    def node_name(self):
        return self._id[0]

    @property
    def fnode_name(self):
        return self._id[1]

    def to_json_str(self):
        return "{}###{}".format(self._id[0], self._id[1] or "")

    @classmethod
    def from_json_str(cls, json_str):
        return cls(json_str.split('###'))

    def __hash__(self):
        return str(self).__hash__()

    def __eq__(self, value):
        return str(self) == str(value)

    def __str__(self):
        if self._id[1]:
            return "_".join(self._id)
        return self._id[0]
