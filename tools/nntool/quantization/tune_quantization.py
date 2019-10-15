# # Copyright (C) 2019 GreenWaves Technologies
# # All rights reserved.

# # This software may be modified and distributed under the terms
# # of the BSD license.  See the LICENSE file for details.

# # Input: threshold
# # For each input file:
# #   evaluate each layer with full accuracy input
# #   while any output error is above threshold
# #       pick layer with lowest QSNR connected to out layer
# #       promote quantization (for conv dp acc then dp all)

# import logging

# import numpy as np

# from utils.stats_funcs import qsnr

# from ..quantization import dequantize
# from ..types import OutputParameters

# from .auto_quantify import auto_propagate

# LOG = logging.getLogger('nntool.'+__name__)

# def tune_quantization(G, in_tensors, max_error, value_cache, orders=None):
#     for in_tensor_idx, in_tensor in enumerate(in_tensors):
#         LOG.info("evaluating input %s", in_tensor_idx)
#         while True:
#             out_errors = []
#             node_qsnrs = {}
#             for step_idx, node, output, qoutput, is_fusion in\
#                 G.execute_comparison_iterator([in_tensor], orders=orders, value_cache=value_cache):
#                 del step_idx, is_fusion
#                 qoutput = [dequantize(out, node.out_q) for out in qoutput]
#                 if isinstance(node, OutputParameters):
#                     out_errors.append((node, np.max(np.abs(output[0] - qoutput[0]))))
#                 elif node.can_promoteq:
#                     node_qsnrs[node] = qsnr(output[0], qoutput[0])
#             # find out node that exceeds error
#             max_node = max(out_errors, key=lambda x: x[1])
#             # if none then break
#             if not node_qsnrs:
#                 # output warning
#                 LOG.warning("unable to meet error threshold for input %s", in_tensor_idx)
#                 break
#             if max_node[1] < max_error:
#                 # output success
#                 LOG.info("success. error %s meets threshold for input %s",
#                          max_node[1], in_tensor_idx)
#                 break
#             LOG.info("node %s has error %s",
#                      max_node[0].name, max_node[1])
#             # find predecessors of that node
#             pre_list = G.all_predecessors(max_node[0].name)
#             # find min qsnr in node_qsnrs
#             # TODO - should really experiment here. try different nodes effect on output
#             pre_qsnrs = [(node, node_qsnrs[node]) for node in pre_list if node in node_qsnrs]
#             min_node = min(pre_qsnrs, key=lambda x: x[1])
#             LOG.info("modifying node %s with qsnr %s",
#                      min_node[0].name, min_node[1])
#             # promote node quantization
#             if min_node[0].promoteq():
#                 LOG.info("node %s's output is now %s bits",
#                         min_node[0].name, min_node[0].out_q.bits)
#                 auto_propagate(G)
