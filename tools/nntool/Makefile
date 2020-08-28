
SCHEMA_V3=schema_v3
SCHEMA_HEAD=schema
SCHEMA_HEAD_NAME=schema_head
PACKAGES=build/packages
FLAT_SRC=$(PACKAGES)/flatbuffers
FLAT_BUILD=$(FLAT_SRC)/build
TFLITE_DIR=importer/tflite
FLAT_TAG=de89bd193370c8b33686f1f33edd63593e48cd3f

all: flatbuffers tflite req

build:
	mkdir -p $@

build/schemas:
	mkdir -p $@

$(PACKAGES): build
	mkdir -p $@

build/.flatbuffers_clone: $(PACKAGES)
	(cd $(PACKAGES); git clone https://github.com/google/flatbuffers.git; cd flatbuffers; git checkout $(FLAT_TAG))
	touch build/.flatbuffers_clone

$(FLAT_BUILD): build/.flatbuffers_clone
	mkdir -p $@

$(FLAT_BUILD)/Makefile: $(FLAT_BUILD)
	(cd $(FLAT_BUILD); cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLATBUFFERS_BUILD_TESTS=OFF ..)

$(FLAT_BUILD)/flatc: $(FLAT_BUILD)/Makefile
	make -C $(FLAT_BUILD) -j

flatbuffers/__init__.py: $(FLAT_BUILD)/flatc
	cp -r $(FLAT_SRC)/python/flatbuffers .

flatbuffers: flatbuffers/__init__.py

tflite: $(SCHEMA_HEAD_NAME)
# tflite: $(SCHEMA_V3) $(SCHEMA_HEAD_NAME)

define schema
  $1: $(TFLITE_DIR)/$1/__init__.py

  $(TFLITE_DIR)/$1/__init__.py: $(FLAT_BUILD)/flatc build/$1/$1.fbs
	$(FLAT_BUILD)/flatc -p -o $(TFLITE_DIR)/ -I $(TFLITE_DIR)/ build/$1/$1.fbs

  build/$1/$1.fbs: build/schemas/$2.fbs build/$1
	sed 's/namespace tflite;/namespace tflite_$1;/g' $$< > $$@

  build/$1: build
	mkdir -p $$@

  build/schemas/$2.fbs: build/schemas
	(cd $$<; wget https://raw.githubusercontent.com/tensorflow/tensorflow/master/tensorflow/lite/schema/$2.fbs)
endef

# $(eval $(call schema,$(SCHEMA_V3),$(SCHEMA_V3)))
$(eval $(call schema,$(SCHEMA_HEAD_NAME),$(SCHEMA_HEAD)))

schema_ui:
	mkdir schema_ui

schema_ui/tflite_$(SCHEMA_HEAD_NAME): build/$(SCHEMA_HEAD_NAME)/$(SCHEMA_HEAD_NAME).fbs
	$(FLAT_BUILD)/flatc --gen-object-api --no-includes --gen-all -p -o schema_ui $<

schema_ui/__init__.py: schema_ui/tflite_$(SCHEMA_HEAD_NAME)
	cat schema_ui/tflite_$(SCHEMA_HEAD_NAME)/*.py > $@

req:
	python3 -m pip install -r requirements.txt

gen_req: clean
	pipreqs --force . 
	pipreqs --print ../nntool_examples >> requirements.txt

clean:
	rm -rf build flatbuffers $(TFLITE_DIR)/tflite_$(SCHEMA_V3) $(TFLITE_DIR)/tflite_$(SCHEMA_HEAD_NAME)

.PHONY: all clean tflite flatbuffers requirements
