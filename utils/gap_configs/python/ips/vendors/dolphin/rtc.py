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

class Rtc(st.Component):

    def __init__(self, parent, name, apb_irq_soc_event, irq_soc_event, calendar_time, calendar_date):

        super(Rtc, self).__init__(parent, name)

        self.add_properties({
            'vp_component': 'vendor/dolphin/rtc_impl',
            'apb_irq_soc_event': apb_irq_soc_event,
            'irq_soc_event': irq_soc_event,
            'calendar_time': calendar_time,
            'calendar_date': calendar_date,
        })