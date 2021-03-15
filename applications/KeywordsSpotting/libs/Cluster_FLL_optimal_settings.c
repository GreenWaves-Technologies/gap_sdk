/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Cluster_FLL_optimal_settings.h"

void __attribute__ ((noinline)) SetFllConfig2(hal_fll_e WhichFll, unsigned int Assert, unsigned int Deassert, unsigned int Tolerance, unsigned int Gain, unsigned int Dithering, unsigned int OpenLoop)
{
        FllConfigT Config;

        Config.Raw = GetFllConfiguration(WhichFll, FLL_CONFIG2);
        Config.ConfigReg2.AssertCycles = Assert;
        Config.ConfigReg2.DeAssertCycles = Deassert;
        Config.ConfigReg2.LockTolerance = Tolerance;
        Config.ConfigReg2.LoopGain = Gain;
        Config.ConfigReg2.Dithering = Dithering;
        Config.ConfigReg2.OpenLoop = OpenLoop;
        SetFllConfiguration(WhichFll, FLL_CONFIG2, (unsigned int) Config.Raw);
}
