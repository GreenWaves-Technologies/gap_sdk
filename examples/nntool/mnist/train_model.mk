# Copyright (C) 2020 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

MODEL_TRAIN = model/train.py
MODEL_CONVERT = model/h5_to_tflite.py
MODEL_H5 = model/$(MODEL_PREFIX).h5
# Increase this to improve accuracy
TRAINING_EPOCHS?=1

$(IMAGES):
	echo "GENERATING INPUT IMAGES"
	(mkdir -p $(IMAGES); $(MODEL_PYTHON) model/save_samples.py -d $@ -n 5)

$(MODEL_H5): 
	$(MODEL_PYTHON) $(MODEL_TRAIN) $@ -e $(TRAINING_EPOCHS)

$(TRAINED_TFLITE_MODEL): $(MODEL_H5) | $(IMAGES)
	$(MODEL_PYTHON) $(MODEL_CONVERT) $< $@
