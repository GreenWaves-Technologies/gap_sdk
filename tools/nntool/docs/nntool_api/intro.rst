NNTool Python API Introduction
------------------------------

The NNTool Python API exposes all of the capabilities in the command line interface and more. It
is very useful for more complicated experiments and graph validation scenarios.

The main interface to the NNTool API is the :class:`nntool.api.NNGraph` class. The :class:`~nntool.api.NNGraph` :meth:`~nntool.api.NNGraph.load_graph`
static method is used to load a graph. It returns an instance of :class:`~nntool.api.NNGraph`.

If a method on a class is not documented here then you should consider it private and subject to change.

Here are some of the relationships between the command line interface and the :class:`~nntool.api.NNGraph` methods.

* *open* - :meth:`~nntool.api.NNGraph.load_graph`
* *adjust* - :meth:`~nntool.api.NNGraph.adjust_order`
* *fusions* - :meth:`~nntool.api.NNGraph.fusions`
* *aquant* and *qtune* - :meth:`~nntool.api.NNGraph.collect_statistics` and :meth:`~nntool.api.NNGraph.quantize`
* *dump* - :meth:`~nntool.api.NNGraph.execute`

Selecting Nodes
^^^^^^^^^^^^^^^

The API refers to operators in the graph as nodes. All nodes inherit from the :class:`nntool.api.types.NNNodeBase` class.

The :class:`~nntool.api.NNGraph` :meth:`~nntool.api.NNGraph.nodes` method can be used to select nodes by node classes.
The :class:`~nntool.api.NNGraph` instance can be indexed by step index or node name to provide the instance of a
particular node in the graph.

e.g.

.. code-block:: python

   from nntool.api import NNGraph
   from nntool.api.types import Conv2DNode

   G = NNGraph.load_graph('mygraph.tflite')

   # get node by step index
   print(G[12].name)

   # get node by name
   print(G["my_conv"].step_idx)

   # get nodes by class. node_classes can be a single class or a tuple of classes
   for node in G.nodes(node_classes=Conv2DNode):
      print(node.name)

Setting Node Options
^^^^^^^^^^^^^^^^^^^^

Most node options are set on nodes through the :attr:`~nntool.api.types.NNNodeBase.at_options`. The options
are identical to those that you can set using the *nodeoptions* command and are exposed as properties of the
NodeOptions class returned from the at_option property.

e.g.

.. code-block:: python

   from nntool.api import NNGraph

   G = NNGraph.load_graph('mygraph.tflite')

   G[12].at_options.dump_tensors = 1

They can also be set and read via the :meth:`nntool.api.NNGraph.set_node_option` and :meth:`nntool.api.NNGraph.get_node_option`
methods.

Collecting Statistics
^^^^^^^^^^^^^^^^^^^^^

To quantize a graph NNTool needs to have statistics on the dynamic of each activation. If the graph is imported with
the load_quantization parameter set to True (i.e. a quantized TFLITE graph or NNCF quantized ONNX graph) then these
statistics will already be present. If not you need to collect them.

The :meth:`nntool.api.NNGraph.collect_statistics` method is used for this. It needs a dataloader which is just an iterable that
returns either normalized input tensors (as numpy float arrays) if there is one input to the graph or a sequence of tensors
if there is more than one input array. :class:`nntool.api.FileImporter` is an example of a dataloader for sounds and images.

Sometimes you just want to see what the performance of a network will be and are not worried about accuracy. In this case
the :class:`nntool.api.RandomIter` class can be used.

e.g.

.. code-block:: python

   from nntool.api import NNGraph
   from nntool.api.utils import RandomIter

   G = NNGraph.load_graph('mygraph.tflite')

   stats = G.collect_statistics(RandomIter.fake(G))

Here is an example of a custom datacollector:

.. code-block:: python

    from PIL import Image
    import numpy as np

    class MyDataLoader():
        def __init__(self, image_files, max_idx=None, transpose_to_chw=True):
            self._file_list = image_files
            self._idx = 0
            self._max_idx = max_idx if max_idx is not None else len(image_files)
            self._transpose_to_chw = transpose_to_chw

        def __iter__(self):
            self._idx = 0
            return self

        def __next__(self):
            if self._idx > self._max_idx:
                raise StopIteration()
            filename = self._file_list[self._idx]

            # Here we read the image and make it a numpy array
            image = Image.open(filename)
            img_array = np.array(image)

            # Apply some preprocessing
            img_array = img_array / 128 - 1.0

            # Transpose to CHW
            if self._transpose_to_chw:
                img_array = img_array.transpose(2, 0, 1)

            self._idx += 1
            return img_array

If you don't want to redo the statistics cache them to disk with numpy.save and numpy.load.

Quantization Options
^^^^^^^^^^^^^^^^^^^^

Quantization options can be set at graph or node level using the node_options or graph_options optional arguments
to the :meth:`~nntool.api.NNGraph.quantize` method. Graph options are just a dict of option name and value. Node options
are a dictionary of node name to dictionary of options. There is a helper function to create the option dictionary
:func:`nntool.api.utils.quantization_options`.

e.g.

.. code-block:: python

   from nntool.api import NNGraph
   from nntool.api.utils import quantization_options

   G = NNGraph.load_graph('mygraph.tflite')

   # ... generate stats

   G.quantize(
      stats,
      node_options={
         'conv1': node_options(weight_bits=2)
      },
      schemes=['scaled'],
      graph_options=node_options(use_ne16=True)
   )

Executing a graph on the simulator or chip
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A loaded graph can be used to build a project and execute the graph on the GVSOC SoC simulator or actually on the chip
directly from the API. The performance data from the execution of the graph can be returned as can all of the tensors
from the execution of the graph. All this functionality is provided through the :class:`~nntool.api.NNGraph` :meth:`~nntool.api.NNGraph.execute_on_target`
method.

Longer example of NNTool Python API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Here is a longer example of some of the functionality in the NNTool Python API.

.. code-block:: python

    from nntool.api import NNGraph

    model = NNGraph.load_graph(
                'face_detection_front.tflite',
                load_quantization=False # Whether tflite quant should be loaded or not (default: False)
            )

    # Model show returns a table of information on the Graph
    print(model.show())

    # Model draw can open or save a PDF with a visual representation of the graph
    model.draw()

    # The equivalent of the adjust command
    model.adjust_order()

    # The equivalent of the fusions --scale8 command. The fusions method can be given a series of fusions to apply
    # fusions('name1', 'name2', etc)
    model.fusions('scaled_match_group')

    # draw the model here again to see the adjusted and fused graph
    model.draw()

    # The executer returns all the layer output. Each layer output is an array of the outputs from each output of a layer
    # Generally layers have one output but some (like a split for example) can have multiple outputs
    # Here we select the first output of the last layer which in a graph with one output will always be the the
    # graph output
    data_loader = MyDataLoader(glob("input_images/*"))
    layer_outputs = model.execute(data_loader)
    last_layer = layer_outputs[-1][0]

    # Now let's quantize the graph
    statistics = model.collect_statistics(data_loader)
    # The resulting statistics contain detailed information for each layer
    statistics['input_1']

    name_layer_2 = model[2].name
    # quantize the model. quantization options can be supplied for a layer or for the whole model
    model.quantize(
        statistics,
        schemes=['scaled'], # Schemes present in the graph
        graph_options={
            "use_ne16": True,
            "hwc": False,
            "force_output_size": 16,
        }, # QUANT_OPTIONS applied graph-wise
        node_options={
            name_layer_2: {
                "use_ne16": False,
                "hwc": True
            }
        }, # QUANT_OPTIONS applied layer-wise
        )
    test_image = next(data_loader)

    # Now execute the quantized graph outputing quantized values
    print("execute model without dequantizing data")
    print(model.execute(test_image, quantize=True)[-1][0])

    # Now execute the graph twice with float and quantized versions and compare the results
    print("execute model comparing float and quantized execution and showing Cosine Similarity")
    cos_sim = model.cos_sim(model.execute(test_image), model.execute(test_image, quantize=True, dequantize=True))
    print(cos_sim)

    # the step idx can be used to index the model to find the layer with the worst cos_sim
    model[np.argmin(cos_sim)]



DSP Preprocessing
^^^^^^^^^^^^^^^^^

To attach a DSP preprocessing to one of the network inputs you can provide
the NNTool API a config file equal to the one described in section
:ref:`nntool_section_attach_special_nodes` and attach it with:

.. code-block:: python


    config = {
        "window": "hanning",
        "pad_type": "center",
        "n_fft": 512,
        "n_frames": 401,
        "frame_size": 512,
        "window_size": 400,
        "frame_step": 160,
        "sample_rate": 16000,
        "magsquared": True,
        "n_fbanks": 128,
        "fbank_type": "librosa",
        "n_dct": 0,
        "fmin": 0.0,
        "fmax": None,
        "log_type": "natural",
        "log_offset": 1e-6
    }
    model.insert_dsp_preprocessing(input_node=model[0], dsp_node_type="mfcc", dsp_node_name="dsp_node", config_dict=config)
    model.adjust_order()

    # Then we quantize to float16 by forcing the statistics for that node
    # (since it's going to be float16 it doesn't matter if the numbers are correct)
    statistics["dsp_node"] = {
        "range_in":  [{"min": -1.0, "max": 1.0}] * len(model["dsp_node"].in_dims),
        "range_out": [{"min": -1.0, "max": 1.0}]
    }
    model.quantize(
        statistics,
        graph_options={
            'bits': 8,
            'quantized_dimension': 'channel',
            'use_ne16': use_ne16,
            'softmax_out_8bits': True
        },
        node_options={
            'input_1': {
                'scheme':'FLOAT', 'float_type':'bfloat16'
            },
            'dsp_node': {
                'scheme':'FLOAT', 'float_type':'bfloat16'
            }
        }
    )
