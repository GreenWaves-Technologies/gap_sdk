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
import generators.v1.soc_gen as soc
import generators.v1.cluster_gen as cluster
import math

def get_config(tp):

  padframe              = tp.get('padframe')
  padframe_conf         = None
  if padframe is not None:
    padframe_conf = js.import_config_from_file(tp.get_child_str('padframe/content'), find=True)
  chip_family           = tp.get_child_str('chip_family')
  chip_name              = tp.get_child_str('chip')
  has_pmu           = tp.get('soc/peripherals/pmu') is not None
  has_rtc           = tp.get('soc/peripherals/rtc') is not None or tp.get('soc/rtc') is not None
  has_udma          = tp.get('soc/peripherals/udma') is not None
  has_cluster       = tp.get('cluster') is not None
  has_fc            = tp.get('soc/fc') is not None
  nb_cluster        = tp.get_child_int('cluster/nb_cluster')
  has_fll           = tp.get('soc/peripherals/fll') is not None or tp.get('soc/peripherals/flls') is not None
  has_ddr = tp.get('ddr') is not None
  if chip_family is None:
    chip_family = chip_name


  def get_cluster_name(cid):
    if cid == 0:
      return 'cluster'
    else:
      return 'cluster_%d' % (cid)


  chip = Component(properties=OrderedDict([
    ('name', chip_name),

    ('pulp_chip_family', chip_family),
    ('pulp_chip_version', 0),
    ('boot_from_rom', False),
    ('vp_class', "pulp/chip"),

    ('hal_files', [ "hal/chips/%s/pulp.h" % chip_family.replace('-', '_')]),
    ('archi_files', [ "archi/chips/%s/pulp.h" % chip_family, "archi/chips/%s/memory_map.h" % chip_family, "archi/chips/%s/properties.h" % chip_family, "archi/chips/%s/apb_soc.h" % chip_family ]),
  ]))


  if padframe is not None:
    content = tp.get_child_str('padframe/content')
    if content is not None:
      chip.padframe = Component(properties=OrderedDict([
          ('includes', [ content ])
      ]))
    else:
      chip.padframe = Component(properties=OrderedDict([
        ('includes', [ "%s" % padframe ])
      ]))

    chip.ref_clock = chip.padframe.ref_clock_pad

  chip.soc_clock = Component(properties=OrderedDict([
    ('vp_class', "vp/clock_domain"),
    ('frequency', 50000000)
  ]))


  chip.soc = soc.get_config(tp)

  if has_cluster:
    for cid in range(0, nb_cluster):
      cluster_name = get_cluster_name(cid)

      chip.add_component(
        get_cluster_name(cid),
        cluster.get_config(tp, cid)
      )

      chip.add_component(
        get_cluster_name(cid) + '_clock',
        Component(properties=OrderedDict([
          ('vp_class', "vp/clock_domain"),
          ('frequency', 50000000)
        ]))
      )
      
      chip.get(get_cluster_name(cid) + '_clock').out = chip.get(get_cluster_name(cid)).clock

      if tp.get_child_bool('**/apb_soc_ctrl/has_pmu_bypass'):
        chip.soc.cluster_reset = chip.get(get_cluster_name(cid)).reset
      if has_fll:
        chip.soc.set(get_cluster_name(cid) + '_fll', chip.get(get_cluster_name(cid) + '_clock').clock_in)

      chip.soc.set(get_cluster_name(cid) + '_input', chip.get(get_cluster_name(cid)).input)

      if has_fc:
        chip.get(cluster_name).dma_irq = chip.soc.dma_irq

      chip.get(cluster_name).soc = chip.soc.soc_input

      chip.ref_clock = chip.get(cluster_name).ref_clock

  # RTC
  if has_rtc:
    rtc_version = tp.get_child_int('**/rtc/version')
    if rtc_version == 1 or rtc_version is None:
      properties = properties=OrderedDict([
        ('includes', ["ips/vendors/dolphin/rtc.json"])
      ])
      properties.update(tp.get('soc/peripherals/rtc').get('config').get_dict())
      chip.rtc = Component(properties=properties)
      chip.rtc.apb_irq = chip.soc.rtc_apb_irq
    else:
      chip.rtc = Component(OrderedDict([
        ('includes', ["ips/rtc/rtc_v%d.json" % rtc_version])
      ]))

    chip.rtc.irq = chip.soc.wakeup_rtc
    chip.rtc.event = chip.soc.rtc_event_in
    if has_fc:
      chip.ref_clock = chip.rtc.ref_clock
    chip.soc.rtc_input = chip.rtc.input
    chip.soc_clock.out = chip.rtc.clock


  if has_pmu:
    pmu_content = tp.get_child_str('soc/peripherals/pmu/content')

    if pmu_content is not None:
      chip.pmu = Component(OrderedDict([
        ('includes', [ pmu_content ])
      ]))
    else:
      chip.pmu = Component(OrderedDict([
        ('includes', ["ips/pmu/pmu_v%d.json" % tp.get_child_str('soc/peripherals/pmu/version')])
      ]))
    chip.soc.pmu_input = chip.pmu.input
    if not tp.get_child_bool('**/apb_soc_ctrl/has_pmu_bypass'):
      chip.pmu.icu6_reset = chip.get(get_cluster_name(cid)).reset
      chip.pmu.icu5_reset = chip.soc.reset
    else:
      chip.pmu.icu0_reset = chip.soc.reset

    chip.pmu.event = chip.soc.event
    if tp.get_child_int('soc/peripherals/pmu/version') >= 3:
      chip.pmu.scu_ok = chip.soc.scu_ok
      chip.pmu.picl_ok = chip.soc.picl_ok
    chip.soc_clock.out = chip.pmu.clock
    chip.soc.wakeup_out = chip.pmu.wakeup
    chip.soc.wakeup_seq = chip.pmu.wakeup_seq
    if has_pmu:
      chip.ref_clock_engine = chip.pmu.ref_clock

  if padframe is not None:
    chip.soc_clock.out = chip.padframe.clock
    if has_fc:
      chip.padframe.ref_clock = chip.soc.ref_clock

    if padframe_conf is not None:
      groups_conf = padframe_conf.get('groups')
      if groups_conf is not None:
        for name, group in groups_conf.get_items().items():
          pad_type = group.get_child_str('type')
          nb_cs = group.get_child_int('nb_cs')
          is_master = group.get_child_bool('is_master')
          is_slave = group.get_child_bool('is_slave')
          is_dual = group.get_child_bool('is_dual')

          if pad_type == 'gpio':
              chip.padframe.set(name + '_pad', chip.soc.new_itf(name))
          else:
            if is_master:
              chip.soc.set(name, chip.padframe.new_itf(name))
              if is_dual:
                chip.padframe.set(name + '_in', chip.soc.new_itf(name + '_in'))
            if is_slave:
              chip.padframe.set(name, chip.soc.new_itf(name))
              if is_dual:
                chip.soc.set(name + '_out', chip.padframe.new_itf(name + '_out'))

          if nb_cs is not None:
            for cs in range(0, nb_cs):
              cs_name = name + '_cs' + str(cs)
              cs_data_name = name + '_cs' + str(cs) + '_data'
              if is_master:
                chip.padframe.set(cs_data_name + '_pad', chip.new_itf(cs_data_name))
                chip.padframe.set(cs_name + '_pad', chip.new_itf(cs_name))
              if is_slave:
                chip.set(cs_data_name, chip.padframe.new_itf(cs_data_name + '_pad'))
                chip.set(cs_name, chip.padframe.new_itf(cs_name + '_pad'))
          else:
            if is_master:
              chip.padframe.set(name + '_pad', chip.new_itf(name))
            if is_slave:
              chip.set(name, chip.padframe.new_itf(name + '_pad'))


  if chip_name == 'wolfe' or chip_name == 'vega':
    chip.bootsel = chip.soc.bootsel

    
  chip.soc_clock.out = chip.soc.clock

  if has_fll:
    chip.soc.fll_soc_clock = chip.soc_clock.clock_in

  if has_ddr:

    chip.soc.ddr = chip.ddr


  config = chip.get_json_config()

  if tp.get('soc/job_fifo') is not None:
      chip.soc.job_fifo_irq = chip.cluster.job_fifo_irq

  return chip
