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
import collections

def get_config(tp):

  chip              = tp.get_child_str('chip')
  has_cluster       = tp.get('cluster') is not None
  nb_cluster        = tp.get_child_int('cluster/nb_cluster')
  nb_pe             = tp.get_child_int('cluster/nb_pe')
  has_fc            = tp.get('soc/fc') is not None
  has_fc_ico        = tp.get('soc/fc_ico') is not None
  has_fc_eu         = tp.get('soc/fc_ico/peripherals/fc_eu') is not None
  has_fc_tcdm       = tp.get('soc/fc_ico/peripherals/fc_tcdm') is not None
  has_ddr           = tp.get('soc/ddr') is not None
  has_rtc           = tp.get('soc/peripherals/rtc') is not None or tp.get('soc/rtc') is not None
  rtc_version       = tp.get_child_int('soc/rtc/version')
  has_udma          = tp.get('soc/peripherals/udma') is not None
  udma_conf         = None
  if has_udma:
    udma_conf = js.import_config_from_file(tp.get_child_str('soc/peripherals/udma/content'), find=True)
  has_spi_master    = tp.get('soc/peripherals/spi_master') is not None
  has_uart          = tp.get('soc/peripherals/uart') is not None
  has_gpio          = tp.get('soc/peripherals/gpio') is not None
  has_soc_events    = tp.get('soc/peripherals/soc_eu') is not None
  if has_fc_eu:
    fc_events         = tp.get('soc/fc_ico/peripherals/fc_eu/irq')
  else:
    fc_events         = tp.get('soc/peripherals/fc_itc/irq')
  l2_is_partitioned = tp.get_child_bool('soc/l2/is_partitioned')
  has_fll           = tp.get('soc/peripherals/fll') is not None or tp.get('soc/peripherals/flls') is not None
  has_rom           = tp.get('soc/rom') is not None
  has_mram          = tp.get('soc/mram') is not None
  has_efuse         = tp.get('soc/peripherals/efuse') is not None
  has_pmu           = tp.get('soc/peripherals/pmu') is not None
  has_hwme          = tp.get('soc/peripherals/hwme') is not None
  has_fc_icache     = tp.get('**/peripherals/fc_icache') is not None
  taps_conf         = tp.get('soc/taps')

  comps = {}

  if fc_events is not None:
    fc_events_dict = fc_events.get_dict()
  else:
    fc_events_dict = OrderedDict()


  fc_itc_name = 'fc_eu' if has_fc_eu else 'fc_itc'


  if has_soc_events:
    soc_events_ids = tp.get('soc_events').get_dict()
  else:
    soc_events_ids = OrderedDict()

  def get_cluster_name(cid):
    if cid == 0:
      return 'cluster'
    else:
      return 'cluster_%d' % (cid)

  soc = Component(properties=OrderedDict([
      ('nb_cluster', nb_cluster),
      ('vp_class', "pulp/soc"),
      ('peripherals_base',  tp.get_child_str("soc/peripherals/base")),
      ('soc_events_ids', soc_events_ids),
      ('fc_events', fc_events_dict)
  ]))

  soc.periph_clock = Component(properties=OrderedDict([
    ('vp_class', "vp/clock_domain"),
    ('frequency', 50000000)
  ]))

  axi_ico_mappings = OrderedDict([
    ("soc", get_mapping(tp.get_child_dict("soc")))
  ])

  if has_cluster:
    for cid in range(0, nb_cluster):
      cluster_name = get_cluster_name(cid)
      base = tp.get_child_int("cluster/base")
      size = tp.get_child_int("cluster/size")
      axi_ico_mappings[cluster_name] = OrderedDict([
        ("base", "0x%x" % (base + size * cid)),
        ("size", "0x%x" % (size))
      ])

  if has_ddr:
    axi_ico_mappings["ddr"] = get_mapping(tp.get_child_dict('soc/ddr'), True)

  soc.axi_ico = Component(properties=OrderedDict([
      ('includes', ["ips/interco/router.json"]),
      ('latency', 9),
      ('mappings', axi_ico_mappings)
  ]))


  soc.soc_ico = Component(properties=OrderedDict([
      ('nb_l2_shared_banks', tp.get_child_int("soc/l2/shared/nb_banks")),
      ('gv_class', "pulp.Soc_Ico_v2.Soc_Ico"),
      ('vp_class', None)
  ]))

  if has_fc_ico:
    fc_ico_mappings = OrderedDict()

  ll_ico_mappings = OrderedDict(OrderedDict([
    ("apb", get_mapping(tp.get_child_dict("soc/peripherals")))
  ]))

  if has_rom:
    ll_ico_mappings.update(OrderedDict([
      ("rom", get_mapping(tp.get_child_dict("soc/rom")))
    ]))

  if has_fc_ico:
    ll_ico_mappings.update(OrderedDict([
      ("fc_ico", get_mapping(tp.get_child_dict("soc/fc_ico"))),
      ("fc_ico_alias", get_mapping(tp.get_child_dict("soc/fc_ico/alias"), add_offset=tp.get_child_int("soc/fc_ico/base") - tp.get_child_int("soc/fc_ico/alias/base"))),
    ]))

  if has_cluster:
    ll_ico_mappings.update(OrderedDict([
      ("axi_master", OrderedDict([
        ("base", tp.get_child_str("cluster/base")),
        ("size", "0x%x" % (tp.get_child_int("cluster/size") * nb_cluster))
      ]))
    ]))

  if l2_is_partitioned:
    ll_ico_mappings.update(OrderedDict([
      ("l2_priv0", get_mapping(tp.get_child_dict("soc/l2/priv0"), True)),
      ("l2_priv0_alias", get_mapping(tp.get_child_dict("soc/l2/priv0_alias"))),
      ("l2_priv1", get_mapping(tp.get_child_dict("soc/l2/priv1"), True)),
      ("l2_shared", get_mapping(tp.get_child_dict("soc/l2/shared"))),
    ]))
  else:
    mapping = get_mapping(tp.get_child_dict("soc/l2"), True)
    if has_fc_ico:
      mapping['latency'] = 8
    ll_ico_mappings.update({
      "l2"         : mapping,
    })

  soc.soc_ico.ll_ico = Component(properties=OrderedDict([
    ('includes', [ "ips/interco/router.json" ]),
    ('mappings', ll_ico_mappings)
  ]))

  soc.soc_ico.hb_ico = Component(properties=OrderedDict([
    ('includes', [ "ips/interco/interleaver.json" ]),
    ('nb_slaves', tp.get_child_int("soc/l2/shared/nb_banks")),
    ('interleaving_bits', tp.get_child_int("soc/l2/shared/interleaving_bits")),
    ('remove_offset', tp.get_child_str("soc/l2/shared/base"))
  ]))

  if has_fc:

    latency = 0
    if has_fc_ico:
      latency = 5

    soc.soc_ico.fc_fetch_ico = Component(properties=OrderedDict([
      ('includes', [ "ips/interco/router.json" ]),
      ('latency', latency),
      ('mappings', OrderedDict([
        ("l2_shared", get_mapping(tp.get_child_dict("soc/l2/shared"))),
        ("ll_ico", OrderedDict())
      ]))
    ]))

    soc.soc_ico.fc_data_ico = Component(properties=OrderedDict([
      ('includes', [ "ips/interco/router.json" ]),
      ('mappings', OrderedDict([
        ("l2_shared", get_mapping(tp.get_child_dict("soc/l2/shared"))),
        ("ll_ico", OrderedDict())
      ]))
    ]))

  if has_udma:
    soc.soc_ico.udma_rx_ico = Component(properties=OrderedDict([
      ('includes', [ "ips/interco/router.json" ]),
      ('mappings', OrderedDict([
        ("l2_shared", get_mapping(tp.get_child_dict("soc/l2/shared"))),
        ("ll_ico", OrderedDict())
      ]))
    ]))

    soc.soc_ico.udma_tx_ico = Component(properties=OrderedDict([
      ('includes', [ "ips/interco/router.json" ]),
      ('mappings', OrderedDict([
        ("l2_shared", get_mapping(tp.get_child_dict("soc/l2/shared"))),
        ("ll_ico", OrderedDict())
      ]))
    ]))

  soc.soc_ico.debug                  = soc.soc_ico.ll_ico.input
  soc.soc_ico.axi_slave              = soc.soc_ico.ll_ico.input
  soc.soc_ico.hb_ico.out_0           = soc.soc_ico.l2_shared_0
  soc.soc_ico.hb_ico.out_1           = soc.soc_ico.l2_shared_1
  soc.soc_ico.hb_ico.out_2           = soc.soc_ico.l2_shared_2
  soc.soc_ico.hb_ico.out_3           = soc.soc_ico.l2_shared_3

  if has_fc:
    soc.soc_ico.fc_fetch               = soc.soc_ico.fc_fetch_ico.input
    soc.soc_ico.fc_data                = soc.soc_ico.fc_data_ico.input
    soc.soc_ico.fc_fetch_ico.l2_shared = soc.soc_ico.hb_ico.input
    soc.soc_ico.fc_fetch_ico.ll_ico    = soc.soc_ico.ll_ico.input
    soc.soc_ico.fc_data_ico.l2_shared  = soc.soc_ico.hb_ico.input
    soc.soc_ico.fc_data_ico.ll_ico     = soc.soc_ico.ll_ico.input

  if has_udma:
    soc.soc_ico.udma_tx                = soc.soc_ico.udma_tx_ico.input
    soc.soc_ico.udma_rx_ico.l2_shared  = soc.soc_ico.hb_ico.input
    soc.soc_ico.udma_rx_ico.ll_ico     = soc.soc_ico.ll_ico.input
    soc.soc_ico.udma_tx_ico.l2_shared  = soc.soc_ico.hb_ico.input
    soc.soc_ico.udma_tx_ico.ll_ico     = soc.soc_ico.ll_ico.input
  soc.soc_ico.ll_ico.apb             = soc.soc_ico.apb
  if has_rom:
    soc.soc_ico.ll_ico.rom             = soc.soc_ico.apb

  if l2_is_partitioned:
    soc.soc_ico.ll_ico.l2_priv0        = soc.soc_ico.l2_priv0
    soc.soc_ico.ll_ico.l2_priv0_alias  = soc.soc_ico.l2_priv0
    soc.soc_ico.ll_ico.l2_priv1        = soc.soc_ico.l2_priv1
    soc.soc_ico.ll_ico.l2_shared       = soc.soc_ico.hb_ico.input
  else:
    soc.soc_ico.ll_ico.l2            = soc.soc_ico.l2

  if has_cluster:
    soc.soc_ico.ll_ico.axi_master      = soc.soc_ico.axi_master





  apb_soc_mappings = OrderedDict([
    ("stdout", get_mapping(tp.get_child_dict("soc/peripherals/stdout"), True)),
    ("apb_soc_ctrl", get_mapping(tp.get_child_dict("soc/peripherals/apb_soc_ctrl"), True)),
  ])

  if has_soc_events:
    apb_soc_mappings.update({
      "soc_eu"       : get_mapping(tp.get_child_dict("soc/peripherals/soc_eu"), True),
    })

  if has_gpio:
    apb_soc_mappings.update({
      "gpio"         : get_mapping(tp.get_child_dict("soc/peripherals/gpio"), True)
    })

  if has_fll:

    flls_config = tp.get('soc/peripherals/flls')

    if flls_config is None:
      apb_soc_mappings.update(OrderedDict([
        ("fll", get_mapping(tp.get_child_dict("soc/peripherals/fll"), True)),
        ("fll1", get_mapping(tp.get_child_dict("soc/peripherals/fll1"), True)),
        ("fll2", get_mapping(tp.get_child_dict("soc/peripherals/fll2"), True))
      ]))
    else:
      for fll_name, fll_config in flls_config.get_items().items():
        apb_soc_mappings.update({
          fll_name          : get_mapping(fll_config.get_dict(), True)
        })

  if has_udma:
    apb_soc_mappings.update({
      "udma"         : get_mapping(tp.get_child_dict("soc/peripherals/udma"), True),
    })

  if has_fc_icache:
    if has_fc_ico:
      fc_ico_mappings.update({
        "fc_icache"       : get_mapping(tp.get_child_dict("**/peripherals/fc_icache"), True)
      })
    else:
      apb_soc_mappings.update({
        "fc_icache"       : get_mapping(tp.get_child_dict("**/peripherals/fc_icache"), True)
      })

  if has_fc:
    if has_fc_ico:
      fc_ico_mappings.update({
        "fc_eu"       : get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/%s" % fc_itc_name), True)
      })
      fc_ico_mappings.update({
        "fc_eu_alias"       : get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/fc_eu_alias"), True)
      })
      fc_ico_mappings.update(OrderedDict([
        ("fc_debug", get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/fc_dbg_unit"), True)),
        ("fc_timer", get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/fc_timer"), True)),
        ("fc_dbg_unit", get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/fc_dbg_unit"), True)),
      ]))
    else:
      apb_soc_mappings.update({
        "fc_itc"       : get_mapping(tp.get_child_dict("soc/peripherals/%s" % fc_itc_name), True)
      })
      apb_soc_mappings.update(OrderedDict([
        ("fc_debug", get_mapping(tp.get_child_dict("soc/peripherals/fc_dbg_unit"), True)),
        ("fc_timer", get_mapping(tp.get_child_dict("soc/peripherals/fc_timer"), True)),
        ("fc_dbg_unit", get_mapping(tp.get_child_dict("soc/peripherals/fc_dbg_unit"), True)),
      ]))

  if has_fc_tcdm:
    fc_ico_mappings.update({
      "fc_tcdm"       : get_mapping(tp.get_child_dict("soc/fc_ico/peripherals/fc_tcdm"), True)
    })

  if has_efuse:
    apb_soc_mappings.update({
      "efuse"           : get_mapping(tp.get_child_dict("soc/peripherals/efuse"), True)
    })

  if has_pmu:
    apb_soc_mappings.update({
      "pmu"       : get_mapping(tp.get_child_dict("soc/peripherals/pmu"), True),
    })

  if has_rom:
    apb_soc_mappings.update({
      "rom"       : get_mapping(tp.get_child_dict("soc/rom"), True),
    })

  if tp.get_child_dict("soc/peripherals/rtc") is not None:
    apb_soc_mappings.update({
      "rtc"       : get_mapping(tp.get_child_dict("soc/peripherals/rtc"), True),
    })
    

  soc.apb_ico = Component(properties=OrderedDict([
      ('includes', [ "ips/interco/router.json" ]),
      ('latency', 8),
      ('mappings', apb_soc_mappings)
  ]))

  if has_fc:
    if has_fc_ico:
      soc.fc_ico =  Component(properties=OrderedDict([
        ('includes', ["ips/interco/router.json"]),
        ('mappings', fc_ico_mappings)
      ]))
    else:
      soc.fc_ico = Component(properties=OrderedDict([
          ('l2_alias', True)
      ]))


  if has_fc:
    soc.fc = Component(properties=OrderedDict([
        ('includes', [ "ips/riscv/%s.json" % tp.get_child_str('soc/fc/core') ]),
        ('cluster_id', tp.get_child_int("soc/fc/cluster_id")),
        ('core_id', tp.get_child_int("soc/fc/core_id")),
        ('fetch_enable', tp.get_child_bool("soc/fc/fetch_enable")),
        ('boot_addr', tp.get_child_str("soc/fc/boot_addr"))
    ]))


  if l2_is_partitioned:

    soc.l2 = Component(properties=OrderedDict([
        ('is_partitioned', True),
        ('nb_shared_banks', tp.get_child_int("soc/l2/shared/nb_banks")),
        ('map_base', tp.get_child_str("soc/l2/base")),
        ('map_size', tp.get_child_str("soc/l2/size")),
        ('size', tp.get_child_str("soc/l2/size"))
    ]))

    soc.l2_priv0 = Component(properties=OrderedDict([
        ('size', tp.get_child_int("soc/l2/priv0/size")),
        ('map_base', tp.get_child_str("soc/l2/priv0/base")),
        ('map_size', tp.get_child_str("soc/l2/priv0/size")),
        ('vp_class', "memory/memory")
    ]))

    soc.l2_priv1 = Component(properties=OrderedDict([
        ('size', tp.get_child_int("soc/l2/priv1/size")),
        ('map_base', tp.get_child_str("soc/l2/priv1/base")),
        ('map_size', tp.get_child_str("soc/l2/priv1/size")),
        ('vp_class', "memory/memory")
    ]))

    soc.l2_shared = Component(properties=OrderedDict([
        ('nb_banks', tp.get_child_int("soc/l2/shared/nb_banks")),
        ('size', tp.get_child_int("soc/l2/shared/size")),
        ('map_base', tp.get_child_str("soc/l2/shared/base")),
        ('map_size', tp.get_child_str("soc/l2/shared/size"))
    ]))

    l2_shared_size = tp.get_child_int("soc/l2/shared/size")
    l2_shared_nb_banks = tp.get_child_int("soc/l2/shared/nb_banks")
    for i in range(0, l2_shared_nb_banks):
      soc.add_component(
        'l2_shared_%d' % i,
        Component(properties=OrderedDict([
          ('size', int(l2_shared_size / l2_shared_nb_banks)),
          ('vp_class', "memory/memory")
        ]))
      )

  else:

    soc.l2 = Component(properties=OrderedDict([
        ('size', tp.get_child_int("soc/l2/size")),
        ('map_base', tp.get_child_str("soc/l2/base")),
        ('map_size', tp.get_child_str("soc/l2/size")),
        ('vp_class', "memory/memory")
    ]))

  if has_rom:
    rom_config_dict = collections.OrderedDict([
      ('includes', ["ips/rom/rom_v%d.json" % tp.get_child_int("soc/rom/version")]),
      ('size', tp.get_child_int("soc/rom/size")),
      ('map_base', tp.get_child_str("soc/rom/base")),
      ('map_size', tp.get_child_str("soc/rom/size")),
      ('vp_class', "memory/memory")
    ])
    rom_config = tp.get('soc/rom/config')
    if rom_config is not None:
      rom_config_dict.update(rom_config.get_dict())

    soc.rom = Component(properties=rom_config_dict)

  soc.plt_loader = Component(properties=OrderedDict([
      ('includes', ["tools/plt_loader/plt_loader.json"])
  ]))

  if has_fc:
    if has_fc_ico:
      soc.fc_eu = Component(properties=OrderedDict([
          ('includes', ["ips/event_unit/eu_v%d.json" % (tp.get_child_int("soc/fc_ico/peripherals/%s/version" % fc_itc_name))]),
          ('nb_core', 1)
      ]))
    else:
      soc.fc_itc = Component(properties=OrderedDict([
          ('includes', ["ips/itc/itc_v%d.json" % (tp.get_child_int("soc/peripherals/%s/version" % fc_itc_name))])
      ]))

  if has_fll:

    flls_config = tp.get('soc/peripherals/flls')

    if flls_config is None:
      soc.fll = Component(properties=OrderedDict([
          ('includes', ["ips/fll/fll_v%d.json" % (tp.get_child_int("soc/peripherals/fll/version"))])
      ]))

      soc.fll1 = Component(properties=OrderedDict([
          ('includes', ["ips/fll/fll_v%d.json" % (tp.get_child_int("soc/peripherals/fll/version"))])
      ]))

      soc.fll2 = Component(properties=OrderedDict([
          ('includes', ["ips/fll/fll_v%d.json" % (tp.get_child_int("soc/peripherals/fll/version"))])
      ]))

      soc.ref_clock = soc.fll.ref_clock
      soc.ref_clock = soc.fll1.ref_clock
      soc.ref_clock = soc.fll2.ref_clock

    else:
      for fll_name, fll_config in flls_config.get_items().items():

        soc.add_component(fll_name, Component(properties=OrderedDict([
            ('includes', ["ips/fll/fll_v%d.json" % (fll_config.get_child_int("version"))])
        ])))

        soc.ref_clock = soc.get(fll_name).ref_clock


    soc.fll_ctrl = Component(properties=OrderedDict([
        ('version', tp.get_child_int("soc/peripherals/fll_ctrl/version")),
        ('vp_class', "pulp/fll/fll_ctrl"),
        ('gv_class', "pulp.Fll_ctrl.fll_ctrl")
    ]))


  apb_soc_params = OrderedDict([
    ('includes', ["chips/%s/apb_soc.json" % chip])
  ])

  if has_cluster and has_pmu:
    apb_soc_params['cluster_power_event'] = tp.get_child_int('soc_events/soc_evt_cluster_pok')
    apb_soc_params['cluster_clock_gate_event'] = tp.get_child_int('soc_events/soc_evt_cluster_cg_ok')

  soc.apb_soc_ctrl = Component(properties=apb_soc_params)

  soc.stdout = Component(properties=OrderedDict([
      ('includes', ["ips/stdout/stdout_v%d.json" % tp.get_child_int("soc/peripherals/stdout/version")])
  ]))

  if has_fc_tcdm:
    soc.fc_tcdm = Component(properties=OrderedDict([
        ('size', tp.get_child_int("soc/fc_ico/peripherals/fc_tcdm/size")),
        ('vp_class', "memory/memory")
    ]))


  if has_gpio:
    gpio_config = OrderedDict([
      ('includes', ["ips/gpio/gpio_v%d.json" % tp.get_child_int("soc/peripherals/gpio/version")])
    ])
    if tp.get('soc_events') is not None:
      gpio_config['soc_event'] = tp.get('soc_events').get_int('soc_evt_gpio')

    soc.gpio = Component(properties=gpio_config)

    soc.apb_ico.gpio = soc.gpio.input

    nb_gpio = tp.get_child_int("soc/peripherals/gpio/nb_gpio")
    if nb_gpio is None:
      nb_gpio = 32

    for i in range(0, nb_gpio):
      soc.set('gpio%d' % i, soc.gpio.new_itf('gpio%d' % i))
      if has_pmu and tp.get_int('soc/peripherals/pmu/version') < 3:
        soc.set('gpio%d' % i, soc.apb_soc_ctrl.new_itf('wakeup_gpio%d' % i))

    if has_pmu:
      if tp.get_int('soc/peripherals/pmu/version') == 3:
        soc.set('gpio64', soc.apb_soc_ctrl.new_itf('wakeup_gpio0'))



  for name, config in tp.get("soc/peripherals").get_items().items():
    file = config.get_child_str("file")
    if file is not None:
      apb_soc_mappings.update({
        name : get_mapping(config.get_dict(), True),
      })
      comp_config = OrderedDict([('includes', [ file ])])
      if config.get('config') is not None:
        comp_config.update(config.get('config').get_dict())
      soc.add_component(name, Component(properties=comp_config))
      soc.apb_ico.set(name, soc.get(name).input)


  if has_fc_icache:
    soc.fc_icache_ctrl = Component(properties=OrderedDict([
        ('includes', ["ips/icache_ctrl/icache_ctrl_v%d.json" % tp.get_child_int("**/fc_icache/version")])
    ]))
    soc.fc_icache = Component(properties=OrderedDict([
        ('includes', ["ips/cache/cache.json"])
    ]))

    soc.fc_icache_ctrl.enable = soc.fc_icache.enable
    soc.fc_icache_ctrl.flush = soc.fc_icache.flush
    soc.fc_icache_ctrl.flush_line = soc.fc_icache.flush_line
    soc.fc_icache_ctrl.flush_line_addr = soc.fc_icache.flush_line_addr

  if has_fc:
    if has_fc_ico:
      soc.timer = Component(properties=OrderedDict([
          ('includes', ["ips/timer/timer_v%d.json" % tp.get_child_int("soc/fc_ico/peripherals/fc_timer/version")])
      ]))
    else:
      soc.timer = Component(properties=OrderedDict([
          ('includes', ["ips/timer/timer_v%d.json" % tp.get_child_int("soc/peripherals/fc_timer/version")])
      ]))

  if has_hwme:
    soc.hwme = Component(properties=OrderedDict([
        ('includes', ["ips/hwme/hwme_v%d.json" % tp.get_child_int("soc/peripherals/hwme/version")])
    ]))

  if has_soc_events:
    soc.soc_eu = Component(properties=OrderedDict([
        ('includes', ["ips/soc_eu/soc_eu_v%d.json" % tp.get_child_int("soc/peripherals/soc_eu/version")]),
        ('ref_clock_event', tp.get('soc_events').get('soc_evt_ref_clock').get_int())
    ]))

  if has_udma:
    content = tp.get_child_str('soc/peripherals/udma/content')
    if content is not None:
      soc.udma = Component(properties=OrderedDict([
          ('includes', [ content ])
      ]))
    else:
      soc.udma = Component(properties=OrderedDict([
          ('includes', ["chips/%s/udma.json" % chip])
      ]))

    comps['udma'] = soc.udma

    if has_mram:      
      mram_config_dict = collections.OrderedDict([
        ('includes', ["ips/mram/mram.json"])
      ])
      mram_config = tp.get('soc/mram/config')
      if mram_config is not None:
        mram_config_dict.update(mram_config.get_dict())

      soc.mram = Component(properties=mram_config_dict)

      soc.udma.mram0 = soc.mram.input


  if has_spi_master:
    soc.spi_master = Component(properties=OrderedDict([
        ('includes', ["ips/spi_master/spi_master_v1.json"])
    ]))

  if has_uart:
    soc.apb_uart = Component(properties=OrderedDict([
        ('includes', ["ips/uart/uart_v0.json"])
    ]))

  if has_efuse:
    efuse_conf = tp.get('soc/peripherals/efuse')
    config = OrderedDict([
        ('includes', ["ips/efuse/efuse_v%d.json" % tp.get_child_int("soc/peripherals/efuse/version")])
    ])
    if efuse_conf.get('config') is not None:
      config.update(efuse_conf.get('config').get_dict())
    soc.efuse = Component(properties=config)

  soc.uart = Component(properties=OrderedDict([
      ('version', 1)
  ]))

  if has_gpio:
    if has_soc_events:
      soc.gpio.event = soc.soc_eu.event_in
    if has_fc:
      fc_irq = fc_events.get_child_int('evt_gpio')
      if fc_irq is not None:
        soc.gpio.irq = soc.fc_itc.new_itf('in_event_%d' % (fc_irq))

  if has_fc and tp.get_child_int("**/fc_dbg_unit/version") <= 1:
    soc.fc_debug = Component(properties=OrderedDict([
        ('version', tp.get_child_int("**/fc_dbg_unit/version"))
    ]))

  if has_cluster:
    for cid in range(0, nb_cluster):
      if tp.get_child_bool('**/apb_soc_ctrl/has_pmu_bypass'):
        soc.apb_soc_ctrl.cluster_reset = soc.cluster_reset


  if taps_conf is None:
    adv_dbg_unit_config = OrderedDict([('includes', ["ips/adv_dbg_unit/adv_dbg_unit.json"])])
    if tp.get('**/adv_dbg_unit/config') is not None:
      adv_dbg_unit_config.update(tp.get('**/adv_dbg_unit/config').get_dict())

    soc.adv_dbg_unit = Component(properties=adv_dbg_unit_config)
  else:

    taps = []

    for tap_name in taps_conf.get_dict():

      tap_template = tp.get('soc').get(tap_name)
      tap_config = tap_template.get('config').get_dict()

      tap = Component(
        properties=tap_template.get('config').get_dict(),
        template=tap_template,
        config=tap_template.get('config')
      )

      soc.add_component(tap_name, tap)

      taps.append(tap)

      if tap_template.get_bool('riscv_debug'):

        debug_rom_config_dict = collections.OrderedDict([
          ('includes', ["ips/rom/rom_v%d.json" % tp.get_child_int("**/debug_rom/version")]),
          ('size', tp.get_child_int("**/debug_rom/size")),
          ('map_base', tp.get_child_str("**/debug_rom/base")),
          ('map_size', tp.get_child_str("**/debug_rom/size")),
          ('vp_class', "memory/memory")
        ])
        rom_config = tp.get('**/debug_rom/config')
        if rom_config is not None:
          debug_rom_config_dict.update(rom_config.get_dict())

        soc.debug_rom = Component(properties=debug_rom_config_dict)

        soc.apb_ico.get_property('mappings')['debug_rom'] = get_mapping(tp.get_child_dict("**/debug_rom"), True)
        soc.apb_ico.get_property('mappings')['debug_rom'] = get_mapping(tp.get_child_dict("**/debug_rom"), True)

        soc.apb_ico.debug_rom = soc.debug_rom.input
        soc.apb_ico.fc_dbg_unit = tap.input 


        tap.set_property('harts', [])

        if has_fc:
          hart_id = (tp.get_int('soc/fc/cluster_id') << 5) | (tp.get_int('soc/fc/core_id'))
          tap.get_property('harts').append([hart_id, 'fc'])
          tap.fc = soc.fc.halt




  # APB to peripherals bindings
  soc.apb_ico.stdout = soc.stdout.input
  if has_fll:

    flls_config = tp.get('soc/peripherals/flls')

    if flls_config is not None:
      for fll_name, fll_config in flls_config.get_items().items():
        soc.apb_ico.set(fll_name, soc.get(fll_name).input)

        if "soc" in fll_config.get('targets').get_dict():
          soc.get(fll_name).clock_out = soc.fll_soc_clock
        elif "cluster" in fll_config.get('targets').get_dict():
          for cid in range(0, nb_cluster):
            soc.get(fll_name).clock_out = soc.new_itf(get_cluster_name(cid) + '_fll')
        elif "periph" in fll_config.get('targets').get_dict():
          soc.get(fll_name).clock_out = soc.periph_clock.clock_in


    else:
      soc.apb_ico.fll = soc.fll.input
      soc.apb_ico.fll1 = soc.fll1.input
      soc.apb_ico.fll2 = soc.fll2.input

      soc.fll1.clock_out = soc.get(get_cluster_name(cid) + '_clock').clock_in
      soc.fll.clock_out = soc.fll_soc_clock


  if has_udma:
    soc.apb_ico.udma = soc.udma.input
    soc.periph_clock.out = soc.udma.periph_clock

  if has_soc_events:
    soc.event = soc.soc_eu.event_in
    soc.apb_ico.soc_eu = soc.soc_eu.input
    
  soc.apb_ico.apb_soc_ctrl = soc.apb_soc_ctrl.input

  if has_rtc:
    soc.wakeup_rtc = soc.apb_soc_ctrl.wakeup_rtc
    if not has_fc_ico:
      soc.wakeup_rtc = soc.fc_itc.in_event_16

    rtc_irq = fc_events.get_child_int('evt_rtc')
    rtc_apb_irq = fc_events.get_child_int('evt_rtc_apb')

    if rtc_irq is not None:
      soc.wakeup_rtc = soc.fc_itc.new_itf('in_event_%d' % (rtc_irq))

    if rtc_apb_irq is not None:
      soc.rtc_apb_irq = soc.fc_itc.new_itf('in_event_%d' % (rtc_apb_irq))

  if has_pmu:
    soc.apb_soc_ctrl.wakeup_out = soc.wakeup_out
    soc.apb_soc_ctrl.wakeup_seq = soc.wakeup_seq


  if has_fc:
    soc.ref_clock = soc.timer.ref_clock

    if has_soc_events:
      soc.ref_clock = soc.soc_eu.ref_clock
      if has_fc_ico:
        soc.soc_eu.ref_clock_event = soc.fc_eu.new_itf('in_event_%d_pe_0' % fc_events.get_child_int('evt_clkref'))
      else:
        soc.soc_eu.ref_clock_event = soc.fc_itc.new_itf('in_event_%d' % fc_events.get_child_int('evt_clkref'))


    if has_fc_ico:
      soc.soc_ico.ll_ico.fc_ico = soc.soc_ico.fc
      soc.soc_ico.fc = soc.fc_ico.input
      soc.soc_ico.ll_ico.fc_ico_alias = soc.soc_ico.fc_alias
      soc.soc_ico.fc_alias = soc.fc_ico.input
      soc.fc_ico.fc_eu_alias = soc.fc_eu.demux_in_0
      soc.fc_ico.fc_eu = soc.fc_eu.input
      soc.fc_ico.fc_timer = soc.timer.input
      if has_fc_icache:
        soc.fc_ico.fc_icache = soc.fc_icache_ctrl.input
      soc.fc_ico.fc_dbg_unit = soc.fc.dbg_unit
    else:
      soc.apb_ico.fc_itc = soc.fc_itc.input
      soc.apb_ico.fc_timer = soc.timer.input
      if has_fc and tp.get_child_int("soc/peripherals/fc_dbg_unit/version") <= 1:
        soc.apb_ico.fc_dbg_unit = soc.fc.dbg_unit
      if has_fc_icache:
        soc.apb_ico.fc_icache = soc.fc_icache_ctrl.input

  if has_fc_tcdm:
    soc.fc_ico.fc_tcdm = soc.fc_tcdm.input

  # Soc interco
  soc.soc_ico.apb = soc.apb_ico.input
  soc.soc_ico.axi_master = soc.axi_ico.input

  if l2_is_partitioned:
    soc.soc_ico.l2_shared_0 = soc.l2_shared_0.input
    soc.soc_ico.l2_shared_1 = soc.l2_shared_1.input
    soc.soc_ico.l2_shared_2 = soc.l2_shared_2.input
    soc.soc_ico.l2_shared_3 = soc.l2_shared_3.input
    soc.soc_ico.l2_priv0 = soc.l2_priv0.input
    soc.soc_ico.l2_priv1 = soc.l2_priv1.input
  else:
    soc.soc_ico.l2 = soc.l2.input

  if has_rtc:
    soc.rtc_event_in = soc.soc_eu.event_in

  if has_rtc:
    if rtc_version is not None and rtc_version >= 2:
      soc.apb_soc_ctrl.rtc = soc.rtc_input
    else:
      soc.apb_ico.rtc = soc.rtc_input

  # APB SOC
  if has_fc:
    soc.apb_soc_ctrl.bootaddr = soc.fc.bootaddr
  if has_udma:
    soc.apb_soc_ctrl.event = soc.soc_eu.event_in
  if has_pmu and has_fc:
    if has_fc_ico:
      soc.apb_soc_ctrl.cluster_power_irq = soc.fc_eu.new_itf('in_event_%d_pe_0' % fc_events.get_child_int('evt_cluster_pok'))
      soc.apb_soc_ctrl.cluster_clock_gate_irq = soc.fc_eu.new_itf('in_event_%d_pe_0' % fc_events.get_child_int('evt_cluster_cg_ok'))
    else:
      if fc_events.get_child_int('evt_cluster_pok') is not None:
        soc.apb_soc_ctrl.cluster_power_irq = soc.fc_itc.new_itf('in_event_%d' % fc_events.get_child_int('evt_cluster_pok'))
        soc.apb_soc_ctrl.cluster_clock_gate_irq = soc.fc_itc.new_itf('in_event_%d' % fc_events.get_child_int('evt_cluster_cg_ok'))

  # ROM
  if has_rom:
    soc.apb_ico.rom = soc.rom.input

  # PMU
  if has_pmu:
    soc.apb_ico.pmu = soc.pmu_input

  # EFUSE
  if has_efuse:
    soc.apb_ico.efuse = soc.efuse.input

  # FC
  if has_fc:
    if has_fc_icache:
      soc.fc.fetch = soc.fc_icache.input_0
      soc.fc_icache.refill = soc.soc_ico.fc_fetch
    else:
      soc.fc.fetch = soc.soc_ico.fc_fetch
    soc.fc.data = soc.soc_ico.fc_data
    if has_fc_ico:
      soc.fc.irq_ack = soc.fc_eu.irq_ack_0
    else:
      soc.fc.irq_ack = soc.fc_itc.irq_ack

  # AXI
  if has_cluster:
    for cid in range(0, nb_cluster):
      cluster_name = get_cluster_name(cid)
      soc.axi_ico.set(cluster_name, soc.new_itf(cluster_name + '_input'))
  soc.axi_ico.soc = soc.soc_ico.axi_slave

  if has_ddr:
    soc.axi_ico.ddr = soc.ddr

  # FC ITC
  if has_fc:
    if has_fc_ico:
      soc.fc_eu.irq_req_0 = soc.fc.irq_req
      soc.fc_eu.clock_0 = soc.fc.clock
    else:
      soc.fc_itc.irq_req = soc.fc.irq_req

  # PMU
  if has_fc:
    if not has_fc_ico:
      soc.scu_ok = soc.fc_itc.in_event_25
      soc.picl_ok = soc.fc_itc.in_event_24

  # Cluster
  if has_cluster:
    if has_fc:
      if has_fc_ico:
        soc.dma_irq = soc.fc_eu.in_event_8_pe_0
      else:
        soc.dma_irq = soc.fc_itc.in_event_8
    soc.soc_input = soc.axi_ico.input


  # Timer
  if has_fc:
    if has_fc_ico:
      soc.timer.irq_itf_0 = soc.fc_eu.in_event_10_pe_0
      soc.timer.irq_itf_1 = soc.fc_eu.in_event_11_pe_0
    else:
      soc.timer.irq_itf_0 = soc.fc_itc.in_event_10
      soc.timer.irq_itf_1 = soc.fc_itc.in_event_11

  # UDMA
  if has_udma:
    soc.udma.l2_itf = soc.soc_ico.udma_tx
    soc.udma.event_itf = soc.soc_eu.event_in
    for itf in udma_conf.get('interfaces').get():
      itf_conf = udma_conf.get(itf.get())
      nb_channels = itf_conf.get_child_str('nb_channels')
      is_master = itf_conf.get_child_bool('is_master')
      is_slave = itf_conf.get_child_bool('is_slave')
      is_dual = itf_conf.get_child_bool('is_dual')
      has_irq = itf_conf.get_child_bool('has_irq')
      for channel in range(0, nb_channels):
        itf_name = itf.get() + str(channel)

        if is_master:
          soc.udma.set(itf_name, soc.new_itf(itf_name))
        if is_slave:
          if is_dual:
            soc.set(itf.get() + str(channel*2), soc.udma.new_itf(itf.get() + str(channel*2)))
            soc.set(itf.get() + str(channel*2+1), soc.udma.new_itf(itf.get() + str(channel*2+1)))
          else:
            soc.set(itf_name, soc.udma.new_itf(itf_name))


  # Soc EU
  if has_fc:
    if has_fc_ico:
      soc.soc_eu.fc_event_itf = soc.fc_eu.soc_event
    else:
      soc.soc_eu.fc_event_itf = soc.fc_itc.soc_event

  if taps_conf is not None:
    soc.jtag0 = taps[0].jtag_in
    taps[-1].jtag_out = soc.jtag0_out
    for index in range(1, len(taps)):
      taps[index -1].jtag_out = taps[index].jtag_in

    for tap in taps:
      if tap.get_config().get_bool('has_io_port'):
        tap.io = soc.soc_ico.debug
      if tp.get_child_bool('**/apb_soc_ctrl/has_jtag_reg'):
        if tap.get_config().get_bool('has_confreg'):
          soc.apb_soc_ctrl.confreg_soc = tap.confreg_soc
          tap.confreg_ext = soc.apb_soc_ctrl.confreg_ext

  # Interrupts
  if has_fc_eu:
    for name, irq in fc_events_dict.items():
      if len(name.split('.')) == 2:
        comp_name, itf_name = name.split('.')
        comps[comp_name].set(itf_name, soc.fc_eu.new_itf('in_event_%d_pe_0' % irq))



  # Loader
  soc.plt_loader.out = soc.soc_ico.debug


  if tp.get('soc/job_fifo') is not None:
      soc.job_fifo = Component(properties=OrderedDict([
          ('includes', ["chips/oprecompkw_sa/job_fifo.json"])
      ]))
      soc.job_fifo_injector = Component(properties=OrderedDict([
          ('includes', ["tools/vp/injector.json"])
      ]))

      soc.host_injector = Component(properties=OrderedDict([
          ('includes', ["tools/vp/injector.json"])
      ]))

      soc.apb_ico.get_property('mappings')["job_fifo"] = OrderedDict([("base", "0x1A120000"), ("size", "0x00001000"), ("remove_offset", "0x1A120000")])
      soc.apb_ico.job_fifo = soc.job_fifo.input
      soc.job_fifo_injector.output = soc.job_fifo.fifo

      soc.axi_ico.get_property('mappings')["ext"] = OrderedDict([
        ("base", "0x1000000000000"), ("size", "0x1000000000000")
      ])
      soc.axi_ico.ext = soc.host_injector.input

      soc.job_fifo.irq = soc.job_fifo_irq

  if chip == 'wolfe' or chip == 'vega':
    soc.bootsel = soc.apb_soc_ctrl.bootsel


  return soc
