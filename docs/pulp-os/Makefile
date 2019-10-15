DOC_BUILDER_DIR=$(GAP_SDK_HOME)/docs/BUILD

all:
	cd dox && make && mkdir -p $(DOC_BUILDER_DIR)/PULP-OS && cp -r doc/runtime/html/* $(DOC_BUILDER_DIR)/PULP-OS

clean:
	rm -rf $(DOC_BUILDER_DIR)/PULP-OS
