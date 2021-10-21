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

from generators.v1.comp_gen import *
import generators.v1.cluster_gen as cluster_gen

class Cluster(cluster_gen.Cluster):

    def __init__(self, tp, cluster_id):
        super(Cluster, self).__init__(tp, cluster_id)

        nb_pe             = tp.get_child_int('cluster/nb_pe')

        for pe in range(0, nb_pe):
            self.cluster.set('halt_pe%d' % pe, self.cluster.get('pe%d' %pe).halt)