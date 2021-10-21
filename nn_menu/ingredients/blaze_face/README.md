# Media Pipe Blaze Face

This is a repository showing Blaze Face face detector + Facial Landmarks on GAP Processors. 

The tflite file comes from [Media Pipe Repository](https://google.github.io/mediapipe/solutions/face_detection.html).

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

