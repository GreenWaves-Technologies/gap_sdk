# Visual Wakewords

This is an implementation of the visual wakewords challenge winner at https://github.com/mit-han-lab/VWW.

The tflite file is a converted version of the optimized uint8 model released by the MIT group.

There are two different builds. The first builds in emulation mode where the AutoTiler generated model is compiled using the host gcc and can be run on the host with sample input. In this mode the calls to the gap SDK are aliased onto the linux API (or ignored). The model runs in a single thread so the cluster cores are not modeled. This mode is interesting for validating that the model is generating correct results and evaluating the real quantization error using the AutoTiler CNN kernels. You can launch this build using the command:

```
make -f emul.mk clean all
```

This produces a executaple file "vww\_emul" which accepts one argument, the image to run the network on:

```
./vww_emul images/COCO_val2014_000000174838_1.ppm 
```

The images have been tagged with the expected output. The \_1 at the end of a filename indicates that there is a person in the image and a \_0 indicates no person. The emul binary also print out the tensors produced at every layer and the actual weights and biases. The AutoTiler may have changed the order of these tensors to reduce the use of 2D DMA transactions from external memory. To disable the print of the tensors you need to set graph_dump_tensor to 0 in the nntool_script_emul.

The second build command builds for GAP but the output can be run on a real GAP development board such as GAPUINO or on the platform simulator GVSOC. Running on GVSOC allows the generation of execution traces. In this mode performance data is generated with the number of cycles used by each layer and the overall graph and the number of MACs executed per cycle.

The build command is:

```
make clean all
```

To run on GVSOC

```
make run platform=gvsoc
```

And on a connected GAPUINO or other GAP development board:

```
make run
```

