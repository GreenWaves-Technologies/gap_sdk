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

from graph.types.activations import (ActivationParameters,
                                     HSigmoidActivationParameters,
                                     HSwishActivationParameters,
                                     LeakyActivationParameters,
                                     ReluActivationParameters,
                                     SoftMaxParameters)
from graph.types.base import (EdgeParameters, FilterLikeParameters,
                              FilterParameters, MultiplicativeBiasParameters,
                              NNEdge, NodeOptions, Parameters,
                              SameNumberOfDimensionsForInputs,
                              SingleInputAndOutput)
from graph.types.conv2d import Conv2DParameters
from graph.types.fusions import (ActivationFusion, ConvFusionParameters,
                                 FusionBase, FusionInputParameters,
                                 FusionOutputParameters,
                                 MatScaleFusionParameters)
from graph.types.image_formatter import ImageFormatParameters
from graph.types.input_output import (ConstantInputParameters,
                                      InputBaseParameters,
                                      InputOutputParameters, InputParameters,
                                      OutputParameters)
from graph.types.linear import FcParameters
from graph.types.lstm import LSTMParameters
from graph.types.others import (CastParameters, ConcatParameters,
                                CopyParameters, GlobalPoolParameters,
                                GroupParameters, NoOPParameters, PadParameters,
                                ReshapeParameters, SplitParameters,
                                StridedSliceParameters, Transposable,
                                TransposeParameters, UnconvertedOpParameters,
                                UnexecutableOpParameters, UnknownOpParameters,
                                UpsampleParameters)
from graph.types.pooling import PoolingParameters
from graph.types.recurrent import RecurrentOutputParameters
from graph.types.rnn import RNNBaseParameters, RNNParameters
from graph.types.ssd import SSDDetectorParameters
from graph.types.tensor_arithmetic import (MatrixAddParameters,
                                           MatrixBroadcastedLinearOpParameters,
                                           MatrixDivParameters,
                                           MatrixMulParameters,
                                           MatrixSubParameters)
