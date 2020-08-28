#
# Copyright (C) 2018 ETH Zurich, University of Bologna and
# GreenWaves Technologies
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

from generators.v1.comp_gen import *
import imp



def get_config(tp):

  chip              = tp.get_child_str('chip')

  system = Component(
  )

  system.system_tree = Empty_Component(OrderedDict([
      ('@includes@', [ "pulp_system_common.json" ]),
      ('vp_class', "pulp/system"),
      ('vp_component', 'utils.composite_impl')
  ]))





  board_name = tp.get_child_str('**/board/name')
  if board_name is None:
    board_name = chip

  system.system_tree.board = Component(OrderedDict([
      ('vp_class', "pulp/board"),
      ('vp_component', 'utils.composite_impl'),
      ('name', board_name)
  ]))


  system.system_tree.board.add_component('chip', Config(
    config=tp.get('board/chip')
  ))

  components = tp.get("board/components")

  if components is not None:
    for comp_name, comp_config in components.items.items():      
      system.system_tree.board.add_component(comp_name, Config(
        config=comp_config
      ))

  bindings = tp.get("board/bindings")

  if bindings is not None:
    for binding in bindings.elems:
      client, server = binding.elems
      client_comp, client_port = client.get().split('.')
      server_comp, server_port = server.get().split('.')

      system.system_tree.board.get(client_comp).set(client_port, system.system_tree.board.get(server_comp).new_itf(server_port))


  devices = {}

  if tp.get('**/devices') is not None:
    devices.update(tp.get('**/devices').items)

  if tp.get('target/board/devices') is not None:
    devices.update(tp.get('target/board/devices').items)

  for device_name, device in devices.items():

    generator = device.get_child_str('generator')
    if generator is None:
      continue

    file, path, descr = imp.find_module(generator, None)
    module = imp.load_module(generator, file, path, descr)

    module.gen_config(device_name, tp, system, device)



  return system.get_js_config(expand=True)
