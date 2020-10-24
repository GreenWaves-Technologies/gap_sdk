artifact.applications:
	rsync -avR $(DEFAULT_EXCLUDES) applications/BilinearResize $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/FaceDetection $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/CannyEdgeDetection $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/MultiScalePedestrianDetector $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/WaterMeter $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/audio_4chan_vocIP $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/jpeg_encoder $(ARTIFACT_PATH)
	rsync -avR $(DEFAULT_EXCLUDES) applications/testset.cfg $(ARTIFACT_PATH)