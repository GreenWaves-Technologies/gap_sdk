NPROC := 4
SCHEMA_V3=schema_v3
PACKAGES=build/packages
FLAT_SRC=$(PACKAGES)/flatbuffers
FLAT_BUILD=$(FLAT_SRC)/build
TFLITE_DIR=importer/tflite2
SCHEMA_HEAD_NAME=tflite_schema_head
FLAT_TAG=v2.0.6
SCHEMA_NAME=schema_v3b
SCHEMA_BASE=https://raw.githubusercontent.com/tensorflow/tensorflow/master/tensorflow/lite/schema/
all: tflite req

build:
	mkdir -p $@

build/schemas:
	mkdir -p $@

$(PACKAGES): build
	mkdir -p $@

build/.flatbuffers_clone: $(PACKAGES)
	(cd $(PACKAGES); git clone https://github.com/google/flatbuffers.git; cd flatbuffers; git checkout $(FLAT_TAG))
	touch build/.flatbuffers_clone

$(FLAT_BUILD)/flatc: build/.flatbuffers_clone
	cmake -S $(FLAT_SRC) -B $(FLAT_BUILD) -DCMAKE_BUILD_TYPE=Release -DFLATBUFFERS_BUILD_TESTS=OFF
	cmake --build $(FLAT_BUILD)

# flatbuffers/__init__.py: $(FLAT_BUILD)/flatc
# 	cp -r $(FLAT_SRC)/python/flatbuffers .

# flatbuffers: flatbuffers/__init__.py

tflite: $(SCHEMA_HEAD_NAME)
# tflite: $(SCHEMA_V3) $(SCHEMA_HEAD_NAME)

define schema
  $1: $(TFLITE_DIR)/$1/__init__.py

  $(TFLITE_DIR)/$1/__init__.py: $(FLAT_BUILD)/flatc build/$1/$1.fbs
	$(FLAT_BUILD)/flatc -p -o $(TFLITE_DIR)/ -I $(TFLITE_DIR)/ build/$1/$1.fbs

  build/$1/$1.fbs: build/schemas/$2.fbs build/$1
	sed 's/namespace tflite;/namespace $1;/g' $$< > $$@

  build/$1: build
	mkdir -p $$@

  build/schemas/$2.fbs: build/schemas
	(cd $$<; wget $(SCHEMA_BASE)/$2.fbs)
endef

# $(eval $(call schema,$(SCHEMA_V3),$(SCHEMA_V3)))
$(eval $(call schema,$(SCHEMA_HEAD_NAME),$(SCHEMA_NAME)))

# schema_ui:
# 	mkdir schema_ui

# schema_ui/$(SCHEMA_HEAD_NAME): build/$(SCHEMA_HEAD_NAME)/$(SCHEMA_HEAD_NAME).fbs
# 	$(FLAT_BUILD)/flatc --gen-object-api --no-includes --gen-all -p -o schema_ui $<

# schema_ui/__init__.py: schema_ui/$(SCHEMA_HEAD_NAME)
# 	cat schema_ui/$(SCHEMA_HEAD_NAME)/*.py > $@

req:
	# Hack to order the requirements
	cat ../requirements.txt | xargs -n 1 -L 1 python3 -m pip install

gen_req: clean
	pipreqs --force . 
	pipreqs --print ../nntool_examples >> ../requirements.txt

vscode_env:
	env | grep TILER > \.env
	env | grep NNTOOL >> \.env
	env | grep GAP_SDK_HOME >> \.env

clean_schema:
	rm -rf build/schemas $(TFLITE_DIR)/$(SCHEMA_HEAD_NAME) $(TFLITE_DIR)/$(SCHEMA_HEAD_NAME)

clean:
	rm -rf build flatbuffers $(TFLITE_DIR)/$(SCHEMA_HEAD_NAME) $(TFLITE_DIR)/$(SCHEMA_HEAD_NAME)

.PHONY: all clean tflite flatbuffers requirements clean_schema
