# Copyright (C) 2017 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

$(IMAGES):
	(mkdir -p $(IMAGES); $(MODEL_PYTHON) model/save_samples.py -d $@ -n 5)

