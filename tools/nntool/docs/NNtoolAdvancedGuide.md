# NNtool execution

The graphs opened and prepared by nntool for the porting on the platform can be tested through the usage of the command line and python scripts. It can be either a fresh new tflite/onnx file or an nntool state previously saved with the `save_state` command.

## Command line

Once you opened your graph or state you can run inference on it using the command line through the `dump` command:

```
nntool
open graph.tflite/onnx/json
dump path/to/image0.jpeg -S float_execution_results
# if graph is quantized
dump path/to/image0.jpeg -S quant_execution_results -q -d
# To compare 1vs1 the results of layer 5 (show to see the layer indexing)
tensors -t float_execution_results quant_execution_results -s 5
```

Through the python shell inside nntool you can inspect the tensors saved in this way and the opened graphs:

```
py
Python 3.8.5 (default, Sep  4 2020, 07:30:14) 
[GCC 7.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.

End with `Ctrl-D` (Unix) / `Ctrl-Z` (Windows), `quit()`, `exit()`.
Non-Python commands can be issued with: app("your command")

>>> tensors['float_execution_results'][5][0]
array([ 4.7394495 ,  2.0431364 ,  2.7551463 ,  0.53899723,  1.7673731 ,
        2.6871662 ,  0.85632235,  2.8186233 , 19.117067  ,  0.7865355 ,
        1.8361021 ,  2.8241088 ,  2.8408155 ,  1.5104855 ,  3.3411233 ,
        0.7706667 , -0.02877633,  2.424552  ,  0.15271291, -0.71261984,
       -2.3913982 , -0.67335063, -0.3945573 ,  2.3752115 , -0.5108621 ,
        2.0935574 , -0.4800231 , -0.6449287 ,  3.0239835 ,  4.532208  ,
        1.0411582 ,  3.488031  ], dtype=float32)
>>> graphs
[{'G': <graph.nngraph.NNGraph object at 0x7fe911b6c8e0>, 'graph_file': '../../../Model_ZOO/ssd_mobilenet_v2_320x320_coco17_tpu-8/model.tflite', 'tensor_file': ''}]
```

## Python script

With the same principle explained above in the py command, you can run a python script which involves the graphs opened in the current session running inference and manipulate input/output in a custom manner:

```python
# my_script.py
from execution.graph_executer import GraphExecuter
from execution.quantization_mode import QuantizationMode # --> nntool modules to run execution

import tensorflow
from your_package import NMS # --> your own packages available in the python environment


QUANTIZE = True
if QUANTIZE:
	qmode = QuantizationMode.all_dequantize()
else:
	qmode = None

dataset_files = glob.glob("path/to/my/dataset/samples/*")

# instantiate an nntool graph executer
executer = GraphExecuter(graphs[0]['G'], qrecs=graphs[0]['G'].quantization)

for file in dataset_files:
	# prepare the inputs in a list of numpy arrays
	input_x = np.array(open(file)) / 255
	outputs = enc_executer.execute([input_x], qmode=qmode, silent=True)

	# estract the outputs indexed by the step number showed by show command
	out1 = outputs[5][0]
	out2 = outputs[14][0]
	out3 = outputs[26][0]

	# use the output to evaluate the model
	NMS(concat(out1, out2))
```

Then to run the python script, after you opened and prepared your nntool graph you can run in the nntool shell:

	nntool
	open graph.json
	run_pyscript my_script.py
