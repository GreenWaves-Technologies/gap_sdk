#
# Copyright (C) 2020 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import gsystree as st

class Router(st.Component):

    def __init__(self, parent, name, latency=0, bandwidth=0, remove_offset=0, config=None):
        super(Router, self).__init__(parent, name)

        self.add_property('mappings', {})
        self.add_property('vp_component', 'interco.router_impl')
        self.add_property('latency', latency)
        self.add_property('bandwidth', bandwidth)
        self.add_property('remove_offset', remove_offset)

        if config is not None:
            self.add_properties(config)

    def add_mapping(self, name, base=None, size=None, remove_offset=None, add_offset=None, id=None):

        mapping = {}

        if base is not None:
            mapping['base'] = base

        if size is not None:
            mapping['size'] = size

        if remove_offset is not None:
            mapping['remove_offset'] = remove_offset

        if add_offset is not None:
            mapping['add_offset'] = add_offset

        if id is not None:
            mapping['id'] = id

        self.get_property('mappings')[name] =  mapping
