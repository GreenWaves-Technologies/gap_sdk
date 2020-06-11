# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from utils.json_serializable import JsonSerializable


class NodeId(JsonSerializable):
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

    def get_node(self, G):
        node = G.node(self._id[0])
        if self._id[1]:
            node = node.get_contained_node(self._id[1])
        return node

    def _encapsulate(self):
        return "{}###{}".format(self._id[0], self._id[1] or "")

    @classmethod
    def _dencapsulate(cls, val):
        return cls(val.split('###'))

    def __hash__(self):
        return str(self).__hash__()

    def __eq__(self, value):
        return str(self) == str(value)

    def __str__(self):
        if self._id[1]:
            return "_".join(self._id)
        return self._id[0]


def convert_node_id_to_str(nodeid):
    if isinstance(nodeid, NodeId):
        return 'NodeId_' + nodeid._encapsulate()
    return nodeid


def convert_str_to_node_id(val):
    if isinstance(val, str) and val.startswith('NodeId_'):
        return NodeId._dencapsulate(val[7:])
    return val


def convert_keys_to_str(info):
    if isinstance(info, list):
        return [convert_keys_to_str(elem) for elem in info]
    if isinstance(info, dict):
        return {convert_node_id_to_str(k): convert_keys_to_str(v) for k, v in info.items()}
    return info


def convert_str_to_keys(info):
    if isinstance(info, list):
        return [convert_keys_to_str(elem) for elem in info]
    if isinstance(info, dict):
        return {convert_str_to_node_id(k):
                convert_str_to_keys(v) for k, v in info.items()}
    return info
