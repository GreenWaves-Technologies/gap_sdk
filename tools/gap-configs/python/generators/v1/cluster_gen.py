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
import math

def get_config(tp, cluster_id):

  cluster_id        = cluster_id
  cluster_size      = tp.get_child_int("cluster/size")
  nb_pe             = tp.get_child_int('cluster/nb_pe')
  l1_banking_factor = tp.get_child_int('cluster/l1/banking_factor')
  cluster_core      = tp.get_child_str('cluster/core')
  cluster_version   = tp.get_child_int('cluster/version')
  has_cc            = tp.get_child_bool('cluster/has_cc')
  has_hwce          = tp.get('cluster/peripherals/hwce') is not None
  has_hwacc          = tp.get('cluster/peripherals/hwacc') is not None
  dma_irq_0         = tp.get('cluster/pe/irq').get_dict().index('dma_0')
  dma_irq_1         = tp.get('cluster/pe/irq').get_dict().index('dma_1')
  dma_irq_ext       = tp.get('cluster/pe/irq').get_dict().index('dma_ext')
  timer_irq_0       = tp.get('cluster/pe/irq').get_dict().index('timer_0')
  timer_irq_1       = tp.get('cluster/pe/irq').get_dict().index('timer_1')
  try:
    job_fifo_irq    = tp.get('cluster/pe/irq').get_dict().index('job_fifo')
  except:
    job_fifo_irq    = None
  #if tp.get('cluster/alias') is not None:
  #  alias = tp.get_child_str('cluster/alias')
  #else:
  #  alias = "0x00000000"

  if has_hwce:
    hwce_irq         = tp.get('cluster/pe/irq').get_dict().index('acc_0')

  if has_hwacc:
    hwacc_irq         = tp.get('cluster/pe/irq').get_dict().index('acc_0')

  core_conf = js.import_config_from_file("ips/riscv/%s.json" % cluster_core, find=True)


  nb_l1_banks = 1<<int(math.log(nb_pe * l1_banking_factor, 2.0))
  l1_bank_size = int(tp.get_child_int('cluster/l1/size') / nb_l1_banks)



  cluster = Component(properties=OrderedDict([
    ('version', cluster_version),
    ('nb_pe', nb_pe),
    ('has_cc', has_cc),
    ('vp_class', "pulp/cluster/cluster"),
    ('vp_component', 'utils.composite_impl'),
  ]))


  axi_ico_mappings = OrderedDict([
    ("soc", get_mapping(tp.get_child_dict("soc")))
  ])

  l1_ts_mapping = get_mapping_area(tp.get_child_dict("cluster/l1"), cluster_size, cluster_id, True)
  l1_ts_mapping['base'] = '0x%x' % (int(l1_ts_mapping['base'], 0) + (1<<20))
  l1_ts_mapping['remove_offset'] = l1_ts_mapping['base']

  cluster.cluster_ico = Component(properties=OrderedDict([
    ('@includes@', ["ips/interco/router.json"]),
    ('latency', 2),
    ('mappings', OrderedDict([
      ("l1", get_mapping_area(tp.get_child_dict("cluster/l1"), cluster_size, cluster_id, True)),
      ("l1_ts", l1_ts_mapping),
      ("periph_ico", get_mapping_area(tp.get_child_dict("cluster/peripherals"), cluster_size, cluster_id)),
      ("periph_ico_alias", get_mapping(tp.get_child_dict("cluster/peripherals/alias"), add_offset=get_area_int('%d' % ((tp.get_child_int("cluster/peripherals/base") - tp.get_child_int("cluster/peripherals/alias/base"))), cluster_size, cluster_id))),
      ("error", OrderedDict([
        ("base", get_area('%d' % tp.get_child_int("cluster/base"), cluster_size, cluster_id)),
        ("size", '0x%x' % cluster_size)
      ])),
      ("soc", OrderedDict())
    ]))
  ]))

  demux_eu_mapping = tp.get_child_dict("cluster/demux_peripherals/event_unit")
  demux_eu_mapping['base'] = '0x%x' % (int(demux_eu_mapping['base'], 0) - tp.get_child_int("cluster/demux_peripherals/base"))

  cluster.demux_periph_ico = Component(properties=OrderedDict([
      ('@includes@', ["ips/interco/router.json"]),
      ('mappings', OrderedDict([
        ("demux_event_unit", get_mapping(demux_eu_mapping)),
      ]))
  ]))

  periph_ico_mappings = OrderedDict()

  for i in range(0, nb_pe):
    size = tp.get_child_int("cluster/peripherals/dbg_unit/size")
    base = tp.get_child_int("cluster/peripherals/dbg_unit/base") + cluster_id*cluster_size + size * i
    periph_ico_mappings['dbg_unit_%d' % i] = OrderedDict([
      ("base", '0x%x' % base),
      ("size", '0x%x' % size),
      ("remove_offset", '0x%x' % base)
    ])

  periph_ico_mappings.update(OrderedDict([
    ("cluster_ctrl", get_mapping_area(tp.get_child_dict("cluster/peripherals/cluster_ctrl"), cluster_size, cluster_id, True)),
    ("timer", get_mapping_area(tp.get_child_dict("cluster/peripherals/timer"), cluster_size, cluster_id, True)),
    ("event_unit", get_mapping_area(tp.get_child_dict("cluster/peripherals/event_unit"), cluster_size, cluster_id, True)),
    ("icache_ctrl", get_mapping_area(tp.get_child_dict("cluster/peripherals/icache_ctrl"), cluster_size, cluster_id, True)),
    ("dma", get_mapping_area(tp.get_child_dict("cluster/peripherals/dma"), cluster_size, cluster_id, True)),
    ("error", OrderedDict([
      ("base", get_area('%d' % tp.get_child_int("cluster/peripherals/base"), cluster_size, cluster_id)),
      ("size", '0x%x' % tp.get_child_int("cluster/peripherals/base"))
    ])),
    ("cluster_ico", OrderedDict())
  ]))

  if has_hwce:
    periph_ico_mappings.update(OrderedDict([
      ("hwce", get_mapping_area(tp.get_child_dict("cluster/peripherals/hwce"), cluster_size, cluster_id, True))
    ]))

  if has_hwacc:
    periph_ico_mappings.update(OrderedDict([
      ("hwacc", get_mapping_area(tp.get_child_dict("cluster/peripherals/hwacc"), cluster_size, cluster_id, True))
    ]))

  cluster.periph_ico = Component(properties=OrderedDict([
    ('@includes@', ["ips/interco/router.json"]),
    ('mappings', periph_ico_mappings)
  ]))

  cluster.l1_ico = Component(properties=OrderedDict([
    ('vp_class', None),
    ('vp_component', ""),
  ]))

  for i in range(0, nb_pe):
    l1_mapping = get_mapping_area(tp.get_child_dict("cluster/l1"), cluster_size, cluster_id, True)
    l1_mapping['id'] = 0

    l1_ts_mapping = get_mapping_area(tp.get_child_dict("cluster/l1"), cluster_size, cluster_id, True)
    l1_ts_mapping['base'] = '0x%x' % (int(l1_ts_mapping['base'], 0) + (1<<20))
    l1_ts_mapping['remove_offset'] = l1_ts_mapping['base']
    l1_ts_mapping['id'] = 0

    l1_alias_mapping = get_mapping(tp.get_child_dict("cluster/l1"), True)
    l1_alias_mapping['base'] = '0x%x' % (int(l1_alias_mapping['base'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    l1_alias_mapping['remove_offset'] = '0x%x' % (int(l1_alias_mapping['remove_offset'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    l1_alias_mapping['id'] = 0

    l1_ts_alias_mapping = get_mapping_area(tp.get_child_dict("cluster/l1"), cluster_size, cluster_id, True)
    l1_ts_alias_mapping['base'] = '0x%x' % (int(l1_ts_alias_mapping['base'], 0) - tp.get_child_int("cluster/base") + (1<<20) + tp.get_child_int("cluster/alias"))
    l1_ts_alias_mapping['remove_offset'] = '0x%x' % (int(l1_ts_alias_mapping['remove_offset'], 0) - tp.get_child_int("cluster/base") + (1<<20) + tp.get_child_int("cluster/alias"))
    l1_ts_alias_mapping['id'] = 0

    eu_mapping = get_mapping_area(tp.get_child_dict("cluster/demux_peripherals/event_unit"), cluster_size, cluster_id, True)
    eu_mapping['id'] = 1

    eu_alias_mapping = get_mapping(tp.get_child_dict("cluster/demux_peripherals/event_unit"), True)
    eu_alias_mapping['base'] = '0x%x' % (int(eu_alias_mapping['base'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    eu_alias_mapping['remove_offset'] = '0x%x' % (int(eu_alias_mapping['remove_offset'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    eu_alias_mapping['id'] = 1

    dma_mapping = get_mapping_area(tp.get_child_dict("cluster/demux_peripherals/dma"), cluster_size, cluster_id, True)
    dma_mapping['id'] = 1

    dma_alias_mapping = get_mapping(tp.get_child_dict("cluster/demux_peripherals/dma"), True)
    dma_alias_mapping['base'] = '0x%x' % (int(dma_alias_mapping['base'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    dma_alias_mapping['remove_offset'] = '0x%x' % (int(dma_alias_mapping['remove_offset'], 0) - tp.get_child_int("cluster/base") + tp.get_child_int("cluster/alias"))
    dma_alias_mapping['id'] = 1

    cluster.l1_ico.add_component(
      'pe%d_ico' % i,
      Component(properties=OrderedDict([
        ('@includes@', ["ips/interco/router.json"]),
        ('mappings', OrderedDict([
          ("l1", l1_mapping),
          ("l1_ts", l1_ts_mapping),
          ("event_unit", eu_mapping),
          ("dma", dma_mapping),
          ("l1_alias", l1_alias_mapping),
          ("l1_ts_alias", l1_ts_alias_mapping),
          ("event_unit_alias", eu_alias_mapping),
          ("dma_alias", dma_alias_mapping),
          ("cluster_ico", { "id": 1 })
        ]))
      ]))
    )

  l1_interleaver_nb_masters = nb_pe + 4
  if has_hwce:
    l1_interleaver_nb_masters += 4
  if has_hwacc:
    l1_interleaver_nb_masters += 4

  cluster.l1_ico.interleaver = Component(properties=OrderedDict([
    ('@includes@', ["ips/interco/l1_interleaver.json"]),
    ('nb_slaves', nb_l1_banks),
    ('nb_masters', l1_interleaver_nb_masters),
    ('interleaving_bits', 2)
  ]))

  cluster.l1_ico.ext2loc = Component(properties=OrderedDict([
    ('@includes@', ["ips/interco/converter.json"]),
    ('output_width', 4),
    ('output_align', 4)
  ]))

  cluster.l1_ico.ext2loc_ts = Component(properties=OrderedDict([
    ('@includes@', ["ips/interco/converter.json"]),
    ('output_width', 4),
    ('output_align', 4)
  ]))

  cluster.l1 = Component(properties=OrderedDict([
    ('vp_class', None),
    ('vp_component', ""),
    ('size', tp.get_child_int("cluster/l1/size")),
    ('alias', True),
    ('has_l1_alias', True),
    ('alias_base', tp.get_child_str("cluster/alias")),
    ('map_base', tp.get_child_str("cluster/l1/base")),
    ('nb_banks', nb_l1_banks),
  ]))

  for i in range(0, nb_l1_banks):
    cluster.l1.add_component(
      'bank%d' % i,
      Component(properties=OrderedDict([
        ('size', l1_bank_size),
        ('width_bits', 2),
        ('vp_class', "memory/memory"),
        ('vp_component', 'memory.memory_impl'),
        ('power_models', {"@includes@": ["power_models/l1/l1.json"] }),
        ('power_trigger', True if i == 0 else False)
      ]))
    )

  cluster.icache = Empty_Component(properties=OrderedDict([
    ('version', 1),
    ('size', 4096*16),
    ('line_size', 16),
    ('banking_factor', 4),
    ('ports', 8),
    ('ways', 1),
    ('enabled', True)
  ]))

  cluster.dma = Component(properties=OrderedDict([
    ('@includes@', ["ips/mchan/mchan_v%d.json" % tp.get_child_int('cluster/peripherals/dma/version')]),
    ('nb_channels', nb_pe+1),
    ('is_64', tp.get_child_bool('cluster/peripherals/dma/is_64') == True)
  ]))

  cluster.cluster_ctrl = Component(properties=OrderedDict([
    ('@includes@', ["ips/cluster_ctrl/cluster_ctrl_v2.json"]),
    ('nb_core', nb_pe)
  ]))

  eu_config = tp.get("cluster/peripherals/event_unit")
  eu_version = eu_config.get_int('version')
  if eu_version is None:
    eu_version = 3
  comp_config = OrderedDict([('@includes@', [ "ips/event_unit/eu_v%d.json" % eu_version ])])
  comp_config['nb_core'] = nb_pe
  if eu_config.get('config') is not None:
    comp_config.update(eu_config.get('config').get_dict())

  cluster.event_unit = Component(properties=comp_config)

  cluster.timer = Component(properties=OrderedDict([
    ('@includes@', ["ips/timer/timer_v2.json"])
  ]))

  if has_hwce:
    cluster.hwce = Component(properties=OrderedDict([
      ('@includes@', ["ips/hwce/hwce_v%d.json" % tp.get_child_int('cluster/peripherals/hwce/version')])
    ]))

  if has_hwacc:
    cluster.hwacc = Component(properties=OrderedDict([
      ('@includes@', ["ips/hwacc/hwacc.json"])
    ]))

  cluster.icache_ctrl = Component(properties=OrderedDict([
    ('@includes@', ["ips/icache_ctrl/icache_ctrl_v2.json"])
  ]))

  icache_config_dict = OrderedDict([
    ('@includes@', ["ips/cache/cache.json"])
  ])

  icache_config = tp.get('cluster/icache/config')
  if icache_config is not None:
    icache_config_dict.update(icache_config.get_dict())

  icache_config_dict['nb_ports'] = nb_pe

  cluster.icache = Component(properties=icache_config_dict)

  cluster.icache_ctrl.enable = cluster.icache.enable
  cluster.icache_ctrl.flush = cluster.icache.flush
  cluster.icache_ctrl.flush_line = cluster.icache.flush_line
  cluster.icache_ctrl.flush_line_addr = cluster.icache.flush_line_addr

  cluster.icache.refill = cluster.cluster_ico.input

  cluster.pe = Empty_Component(properties=OrderedDict(
  ))

  for i in range(0, nb_pe):
    cluster.add_component(
      'pe%d' % i,
      Component(properties=OrderedDict([
        ('@includes@', ["ips/riscv/%s.json" % cluster_core]),
        ('cluster_id', cluster_id),
        ('core_id', i),
        ('fetch_enable', tp.get_child_bool("cluster%d/pe%d/fetch_enable" % (cluster_id, i))),
        ('boot_addr', tp.get_child_str("cluster%d/pe%d/boot_addr" % (cluster_id, i)))
      ]))
    )

    cluster.add_component(
      'pe',
      Empty_Component(properties=OrderedDict([
        ('@includes@', ["ips/riscv/%s.json" % cluster_core]),
        ('cluster_id', cluster_id),
        ('core_id', 0),
        ('boot_addr', "0x1C000000")
      ]))
    )


  cluster.dma.ext_irq_itf = cluster.dma_irq
  cluster.cluster_ico.soc = cluster.soc
  cluster.input = cluster.cluster_ico.input
  cluster.cluster_ico.l1 = cluster.l1_ico.ext2loc_itf
  cluster.cluster_ico.l1_ts = cluster.l1_ico.ext2loc_ts_itf
  cluster.cluster_ico.periph_ico = cluster.periph_ico.input
  cluster.cluster_ico.periph_ico_alias = cluster.periph_ico.input
  cluster.periph_ico.icache_ctrl = cluster.icache_ctrl.input
  if has_hwce:
    cluster.periph_ico.hwce = cluster.hwce.input
  if has_hwacc:
    cluster.periph_ico.hwacc = cluster.hwacc.input
  cluster.periph_ico.event_unit = cluster.event_unit.input
  cluster.periph_ico.cluster_ctrl = cluster.cluster_ctrl.input
  cluster.periph_ico.timer = cluster.timer.input
  cluster.ref_clock = cluster.timer.ref_clock
  cluster.periph_ico.dma = cluster.dma.new_itf("in_%d" % nb_pe)

  if has_hwce:
    for i in range(0, nb_pe):
      cluster.hwce.irq = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (hwce_irq, i))

  if has_hwacc:
    for i in range(0, nb_pe):
      cluster.hwacc.irq = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (hwacc_irq, i))

  for i in range(0, nb_pe):
    cluster.periph_ico.set('dbg_unit_%d' % i, cluster.get('pe%d' % i).dbg_unit)

  for i in range(0, nb_pe):
    cluster.cluster_ctrl.set('bootaddr_%d' % i, cluster.get('pe%d' % i).bootaddr)

  for i in range(0, nb_pe):
    cluster.cluster_ctrl.set('fetchen_%d' % i, cluster.get('pe%d' % i).fetchen)

  for i in range(0, nb_pe):
    cluster.cluster_ctrl.set('halt_%d' % i, cluster.get('pe%d' % i).halt)

  for i in range(0, nb_pe):
    cluster.get('pe%d' % i).halt_status = cluster.cluster_ctrl.new_itf('core_halt_%d' % i)

  for i in range(0, nb_pe):
    cluster.get('pe%d' % i).data = cluster.l1_ico.new_itf('data_pe_%d' % i)

  for i in range(0, nb_pe):
    cluster.get('pe%d' % i).fetch = cluster.icache.new_itf('input_%d' % i)

  for i in range(0, nb_pe):
    cluster.l1_ico.set('event_unit_%d' % i, cluster.event_unit.new_itf('demux_in_%d' % i))

  for i in range(0, nb_pe):
    cluster.l1_ico.set('event_unit_alias_%d' % i, cluster.event_unit.new_itf('demux_in_%d' % i))

  for i in range(0, nb_pe):
    cluster.l1_ico.set('dma_%d'%i, cluster.dma.new_itf('in_%d'%i))

  for i in range(0, nb_pe):
    cluster.l1_ico.set('dma_alias_%d'%i, cluster.dma.new_itf('in_%d'%i))

  for i in range(0, nb_pe):
    cluster.dma.set('event_itf_%d' % i, cluster.event_unit.new_itf('in_event_%d_pe_%d' % (dma_irq_0, i)))

  for i in range(0, nb_pe):
    cluster.dma.set('irq_itf_%d' % i, cluster.event_unit.new_itf('in_event_%d_pe_%d' % (dma_irq_1, i)))

  for i in range(0, nb_pe):
    cluster.dma.ext_irq_itf = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (dma_irq_ext, i))

  for i in range(0, nb_pe):
    cluster.timer.irq_itf_0 = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (timer_irq_0, i))

  for i in range(0, nb_pe):
    cluster.timer.irq_itf_1 = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (timer_irq_1, i))

  for i in range(0, nb_pe):
    cluster.event_unit.set('irq_req_%d' % i, cluster.get('pe%d' % i).irq_req)

  for i in range(0, nb_pe):
    cluster.event_unit.set('clock_%d' % i, cluster.get('pe%d' % i).clock)

  for i in range(0, nb_pe):
    cluster.get('pe%d' % i).irq_ack = cluster.event_unit.new_itf('irq_ack_%d' % i)

    first_ext_counter = core_conf.get_int('first_ext_counter')
    if first_ext_counter is None:
      first_ext_counter = 11

    for j in range(first_ext_counter, first_ext_counter+5):
      cluster.get('pe%d' % i).set('ext_counter[%d]' % j, cluster.l1_ico.new_itf('ext_counter_%d[%d]' % (i, j)))

  cluster.periph_ico.cluster_ico = cluster.cluster_ico.input

  for i in range(0, nb_l1_banks):
    cluster.l1.set('in_%d'%i, cluster.l1.get('bank%d'%i).input)

  for i in range(0, nb_l1_banks):
    cluster.l1_ico.set('out_%d' % i, cluster.l1.new_itf('in_%d' % i))

  cluster.l1_ico.cluster_ico = cluster.cluster_ico.input
  cluster.dma.ext_itf = cluster.cluster_ico.input

  for i in range(0, 4):
    cluster.dma.set('loc_itf_%d' % i, cluster.l1_ico.new_itf('dma_in_%d' % i))
   
  for i in range(0, 4):
    cluster.l1_ico.set('dma_in_%d' % i, cluster.l1_ico.interleaver.new_itf('in_%d' % (nb_pe + i)))

  if has_hwce:
    for i in range(0, 4):
      cluster.hwce.set('out_%d' % i, cluster.l1_ico.new_itf('hwce_in_%d' % i))
      cluster.l1_ico.set('hwce_in_%d' % i, cluster.l1_ico.interleaver.new_itf('in_%d' % (nb_pe + 4 + i)))

  if has_hwacc:
    for i in range(0, 4):
      cluster.hwacc.set('out_%d' % i, cluster.l1_ico.new_itf('hwacc_in_%d' % i))
      cluster.l1_ico.set('hwacc_in_%d' % i, cluster.l1_ico.interleaver.new_itf('in_%d' % (nb_pe + 4 + i)))

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico' % i).dma = cluster.l1_ico.new_itf('dma_%d'%i)

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico' % i).dma_alias = cluster.l1_ico.new_itf('dma_alias_%d'%i)

  for i in range(0, nb_pe):  
    cluster.l1_ico.get('pe%d_ico' % i).event_unit = cluster.l1_ico.new_itf('event_unit_%d'%i)

  for i in range(0, nb_pe):  
    cluster.l1_ico.get('pe%d_ico' % i).event_unit_alias = cluster.l1_ico.new_itf('event_unit_alias_%d'%i)

  for i in range(0, nb_pe):  
    cluster.l1_ico.get('pe%d_ico' % i).cluster_ico = cluster.l1_ico.cluster_ico

  for i in range(0, nb_l1_banks):
    cluster.l1_ico.interleaver.set('out_%d'%i, cluster.l1_ico.new_itf('out_%d'%i))

  cluster.l1_ico.ext2loc_itf = cluster.l1_ico.ext2loc.input
  cluster.l1_ico.ext2loc_ts_itf = cluster.l1_ico.ext2loc_ts.input

  first_external_pcer = core_conf.get_int('first_external_pcer')
  if first_external_pcer is None:
    first_external_pcer = 11

  for i in range(0, nb_pe):
    cluster.l1_ico.set('ext_counter_%d[%d]'%(i, first_external_pcer + 0), cluster.l1_ico.get('pe%d_ico'%i).new_itf('nb_read[1]'))

  for i in range(0, nb_pe):
    cluster.l1_ico.set('ext_counter_%d[%d]'%(i, first_external_pcer + 1), cluster.l1_ico.get('pe%d_ico'%i).new_itf('nb_write[1]'))
    
  for i in range(0, nb_pe):
    cluster.l1_ico.set('ext_counter_%d[%d]'%(i, first_external_pcer + 2), cluster.l1_ico.get('pe%d_ico'%i).new_itf('read_stalls[1]'))
    
  for i in range(0, nb_pe):
    cluster.l1_ico.set('ext_counter_%d[%d]'%(i, first_external_pcer + 3), cluster.l1_ico.get('pe%d_ico'%i).new_itf('write_stalls[1]'))
    
  for i in range(0, nb_pe):
    cluster.l1_ico.set('ext_counter_%d[%d]'%(i, first_external_pcer + 4), cluster.l1_ico.get('pe%d_ico'%i).new_itf('stalls[0]'))

  for i in range(0, nb_pe):
    cluster.l1_ico.set('data_pe_%d'%i, cluster.l1_ico.get('pe%d_ico'%i).input)

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico'%i).l1 = cluster.l1_ico.interleaver.new_itf('in_%d'%i)

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico'%i).l1_alias = cluster.l1_ico.interleaver.new_itf('in_%d'%i)

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico'%i).l1_ts = cluster.l1_ico.interleaver.new_itf('ts_in_%d'%i)

  for i in range(0, nb_pe):
    cluster.l1_ico.get('pe%d_ico'%i).l1_ts_alias = cluster.l1_ico.interleaver.new_itf('ts_in_%d'%i)


  cluster.l1_ico.ext2loc.out = cluster.l1_ico.interleaver.new_itf('in_%d'%nb_pe)
  cluster.l1_ico.ext2loc_ts.out = cluster.l1_ico.interleaver.new_itf('ts_in_%d'%nb_pe)


  if job_fifo_irq is not None:
    for i in range(0, nb_pe):
      cluster.job_fifo_irq = cluster.event_unit.new_itf('in_event_%d_pe_%d' % (job_fifo_irq, i))



  return cluster
