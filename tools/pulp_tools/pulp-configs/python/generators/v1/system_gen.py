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

import generators.v1.chip_gen as chip_gen
from generators.v1.comp_gen import *
import imp



def get_config(tp):

  start_addr = None
  stop_addr = None
  set_pc_addr = None
  set_pc_offset = None
  debug_bridge_commands = None
  chip              = tp.get_child_str('chip')

  if chip in ['pulpissimo', 'pulpissimo_v1', 'pulp', 'pulp_v1']:
      start_addr = '0x%X' % (tp.get_child_int("soc/peripherals/fc_dbg_unit/base") + 0x0000)
      start_value = '0'
      stop_addr = '0x%X' % (tp.get_child_int("soc/peripherals/fc_dbg_unit/base") + 0x0000)
      stop_value = '0x00010000'
      set_pc_addr = '0x%X' % (tp.get_child_int("soc/peripherals/fc_dbg_unit/base") + 0x2000)
      debug_bridge_commands = 'reset stop load ioloop reqloop start wait'

  if chip in ['vivosoc3']:
      start_addr = '0x1A104014'
      start_value = '1'
      stop_addr = '0x%X' % (tp.get_child_int("soc/peripherals/fc_dbg_unit/base") + 0x0000)
      stop_value = '0x00010000'
      set_pc_addr = '0x1A104010'
      debug_bridge_commands = 'reset stop load ioloop reqloop start wait'

  if chip in ['multino', 'bigpulp-standalone', 'bigpulp', 'bigpulp-z-7045', 'oprecompkw_sa']:
      start_addr = '0x%X' % (tp.get_child_int("cluster/peripherals/cluster_ctrl/base") + 0x0008)
      start_value = '1'
      stop_addr = '0x%X' % (tp.get_child_int("cluster/peripherals/cluster_ctrl/base") + 0x0008)
      stop_value = '0x00010000'
      set_pc_addr = '0x%X' % (tp.get_child_int("cluster/peripherals/cluster_ctrl/base") + 0x0040)
      set_pc_offset = "-128"
      debug_bridge_commands = 'reset stop load ioloop reqloop start wait'

  system = Component(
  )

  system.system_tree = Empty_Component(OrderedDict([
      ('includes', [ "pulp_system_common.json" ]),
      ('vp_class', "pulp/system")
  ]))



  debug_bridge_dict = OrderedDict([
      ('includes', ["tools/debug-bridge/debug_bridge.json"])
  ])

  debug_bridge_config = tp.get('**/debug_bridge/config')

  if debug_bridge_config is not None:
    debug_bridge_dict.update(debug_bridge_config.get_dict())

  system.system_tree.debug_bridge = Component(
    properties=debug_bridge_dict
  )



  runner_dict = OrderedDict([
      ('includes', ["tools/runner/runner.json"])
  ])

  runner_config = tp.get('**/runner/config')

  if runner_config is not None:
    runner_dict.update(runner_config.get_dict())

  system.runner = Component(
    properties=runner_dict
  )



  rt_dict = OrderedDict([
      ('includes', ["tools/rt/rt.json"])
  ])

  rt_config = tp.get('**/rt/config')

  if rt_config is not None:
    rt_dict.update(rt_config.get_dict())

  system.rt = Component(
    properties=rt_dict
  )



  if start_addr is not None:
      system.system_tree.debug_bridge.set_property('start_addr', start_addr)
      system.system_tree.debug_bridge.set_property('start_value', start_value)

  if stop_addr is not None:
      system.system_tree.debug_bridge.set_property('stop_addr', stop_addr)
      system.system_tree.debug_bridge.set_property('stop_value', stop_value)

  if set_pc_addr is not None:
      system.system_tree.debug_bridge.set_property('set_pc_addr', set_pc_addr)

  if set_pc_offset is not None:
      system.system_tree.debug_bridge.set_property('set_pc_offset', set_pc_offset)

  if debug_bridge_commands is not None:
      system.system_tree.debug_bridge.set_property('commands', debug_bridge_commands)


  board_name = tp.get_child_str('**/board/name')
  if board_name is None:
    board_name = chip


  system.system_tree.board = Component(OrderedDict([
      ('vp_class', "pulp/board"),
      ('name', board_name)
  ]))



  system.system_tree.board.chip = chip_gen.get_config(tp)

  system.system_tree.board.dpi_clock = Component(OrderedDict([
      ('vp_class', "vp/clock_domain"),
      ('frequency', 50000000)
  ]))

  system.system_tree.board.pulp_chip = Component(
  )
  system.system_tree.board.pulp_chip.set_property(chip, {})


  system.system_tree.board.dpi = Component(OrderedDict([
      ('includes', [ "periph/dpi_wrapper.json" ])
  ]))

  system.system_tree.board.dpi_clock.out = system.system_tree.board.dpi.clock
  system.system_tree.board.dpi.chip_reset = system.system_tree.board.chip.reset


  system.system_tree.board.ref_clock_clock = Component(OrderedDict([
  ('vp_class', "vp/clock_domain"),
  ('frequency', 65536)
  ]))

  system.system_tree.board.ref_clock = Component(OrderedDict([
    ('includes', [ "ips/misc/clock.json" ])
  ]))

  system.system_tree.board.ref_clock_clock.out = system.system_tree.board.ref_clock.clock

  system.system_tree.board.ref_clock.clock_sync = system.system_tree.board.chip.ref_clock

  if tp.get('soc/peripherals/pmu') is not None:
    system.system_tree.board.ref_clock_clock.out = system.system_tree.board.chip.ref_clock_engine


  if chip == 'wolfe' or chip == 'vega':
    system.system_tree.board.bootsel = Component(OrderedDict([
      ('vp_class', "board/switch"),
      ('value', "0")
    ]))

    system.system_tree.board.bootsel.out = system.system_tree.board.chip.bootsel
    system.system_tree.board.dpi.chip_config = system.system_tree.board.bootsel.input



  has_ddr = tp.get('ddr') is not None

  if has_ddr:

    system.system_tree.board.ddr_clock = Component(OrderedDict([
      ('vp_class', "vp/clock_domain"),
      ('frequency', tp.get_child_str("ddr/frequency"))
    ]))

    system.system_tree.board.ddr = Component(OrderedDict([
        ('includes', [ "ips/memory/ddr.json" ]),
        ('size', tp.get_child_str("ddr/size"))
    ]))

    system.system_tree.board.chip.ddr = system.system_tree.board.ddr.input
    system.system_tree.board.ddr_clock.out = system.system_tree.board.ddr.clock


  if tp.get('**/devices') is not None:
    for device_name, device in tp.get('**/devices').items.items():

      generator = device.get_child_str('generator')
      if generator is None:
        raise Exception('No generator specified for device (name: %s)' % device_name)

      file, path, descr = imp.find_module(generator, None)
      module = imp.load_module(generator, file, path, descr)

      module.gen_config(device_name, tp, system, device)



  return system.get_js_config()
