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

from nntool.graph.types.activations import (ActivationNodeBase, HSigmoidNode,
                                            HSwishNode, HTanHNode, LeakyNode,
                                            ReluNode, SigmoidNode, SoftMaxNode,
                                            TanHNode)
from nntool.graph.types.base import (EdgeParameters, FilterLikeNodeBase,
                                     FilterNodeBase,
                                     InsensitiveToQuantizationMixin,
                                     MultiplicativeBiasNodeBase, NNEdge,
                                     NNNodeBase, NodeOptions,
                                     SameNumberOfDimensionsForInputsMixin,
                                     SensitiveToOrderMixin,
                                     SingleInputAndOutputMixin)
from nntool.graph.types.constant_input import ConstantInputNode
from nntool.graph.types.conv2d import (BatchNormalizationNode, Conv2DNode,
                                       TransposeConv2DParameters)
from nntool.graph.types.dsp_preprocessing import (DSPNodeBase,
                                                  MFCCPreprocessingNode,
                                                  RFFT2DPreprocessingNode)
from nntool.graph.types.expression_fusion import ExpressionFusionNode
from nntool.graph.types.fusions import (ActivationFusionNode,
                                        ActivationFusionNodeBase,
                                        BroadcastableActivationFusionNode,
                                        ConvFusionNode, FilterFusionNodeBase,
                                        FusionInputNode, FusionNodeBase,
                                        FusionOutputNode, LinearFusionNode,
                                        MatMulOpFusionNode, MatScaleFusionNode,
                                        PaddedAddFusionNode)
from nntool.graph.types.global_pooling import (GlobalAveragePoolNode,
                                               GlobalMaxPoolNode,
                                               GlobalMinPoolNode,
                                               GlobalPoolingNodeBase,
                                               GlobalSumPoolNode)
from nntool.graph.types.image_formatter import ImageFormatNode
from nntool.graph.types.input_output import (InputNNNodeBase, InputNode,
                                             InputOutputNNNodeBase, OutputNode)
from nntool.graph.types.linear import LinearNode
from nntool.graph.types.lstm import LSTMNode
from nntool.graph.types.misc import (CopyNode, NoOPNode, PadNode, QuantizeNode,
                                     UnconvertedOpNode, UnexecutableNodeBase,
                                     UnknownOpNode)
from nntool.graph.types.piecewise import (ExpOpNode, LogOpNode, NegOpNode,
                                          SqrtOpNode, UnaryOpNodeBase, ErfOpNode)
from nntool.graph.types.pooling import (AveragePoolNode, MaxPoolNode,
                                        PoolingNodeBase)
from nntool.graph.types.resizers import (BilinearResizerNode,
                                         NearestNeighborResizerNode,
                                         ResizerNodeBase)
from nntool.graph.types.rnn import GRUNode, RNNNode, RNNNodeBase
from nntool.graph.types.ssd import NMSNode, SSDDetectorNode
from nntool.graph.types.tensor_arithmetic import (
    BroadcastableNodeBase, MatMulOpNode, MatMulTransposedNode, MatrixAddNode,
    MatrixBroadcastedLinearOpNodeBase, MatrixDivNode, MatrixMulNode,
    MatrixSubNode, MaxOpNode, MinOpNode, PowOpNode)
from nntool.graph.types.tensor_shape import (BatchToSpaceNode, ConcatNode,
                                             ExpandNode, GatherNode,
                                             RepeatNode, ReshapeNode,
                                             ReverseNode, SpaceToBatchNode,
                                             SplitNode, StridedSliceNode,
                                             TransposeNode)
