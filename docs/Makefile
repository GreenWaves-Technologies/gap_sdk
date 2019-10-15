# Makefile for GAP8 SDK documentation
# you must source sourceme.sh in the GAP SDK dir before running this

ifndef GAP_SDK_HOME
	$(error run 'source sourceme.sh' in gap_sdk first)
endif

DOC_DIR_NAME= docs
DOC_DIR= $(GAP_SDK_HOME)/$(DOC_DIR_NAME)
HOME_PAGE=$(DOC_DIR)/gap_doc.html

export COM_BASE=common
DOC_BUILDER_DIR=$(DOC_DIR)/BUILD

RM=rm -f
CP=cp
MKDIR=mkdir -p
CAT=cat
PANDOC=pandoc -N -f markdown -s --template $(DOC_DIR)/$(COM_BASE)/gwt.latex
CD=cd

GAP8HEADER=gap8header
GAP8HEADER_DIR=pulp-os
BUILTINS_DIR= $(GAP8HEADER_DIR)/$(GAP8HEADER)
BUILTINS_HEADERS= builtins_gap8.h builtins_pulp.h

PULP_OS_DIR=$(DOC_DIR)/pulp-os

COMMONNAMES=pulp_header.html pulp_footer.html printComponentTabs.js pulp-logo-55.png greenwaves-logo-55.png gap.css
COMMON= $(addprefix $(COM_BASE)/, $(COMMONNAMES))

# Utilities to play with images to get them to work well in doxygen
GENSTYLE = $(COM_BASE)/extractimgcss.pl
MDPD2DOXY = $(COM_BASE)/md_pandoc_to_doxygen.pl
PATCHJS = $(COM_BASE)/patchjs.pl

TABSCRIPT= printComponentTabs.js
TABSCRIPT_SRC= $(COM_BASE)/$(TABSCRIPT)
export TABSCRIPT_DST= $(BUILD_DIR)/$(TABSCRIPT)

# export and define stuff for doxygen and makefile
# runtime doxygen template
export DOXTEMPLATE=$(PULP_OS_DIR)/dox
# pulp home
export PULP_PROJECT_HOME

export AUTOTILER_SRC=$(GAP_SDK_HOME)/tools/autotiler
export AUTOTILER_GENERATOR_SRC=$(GAP_SDK_HOME)/tools/autotiler/generators
export AUTOTILER_EXAMPLES_SRC=$(GAP_SDK_HOME)/examples/pulp-examples/autotiler_examples

export BUILD_DIR=$(DOC_DIR)/BUILD

export GAP8_BUILTINS=$(addprefix $(BUILD_DIR)/, $(BUILTINS_HEADERS))

export HM_BASE=home
export AT_BASE=autotiler
export RT_BASE=pulp-os/runtime
export APP_BASE=applications
export MB_BASE=mbed-os
export FR_BASE=freertos
export VP_BASE=gvsoc
export BM_BASE=benchmarks
export PMSIS_BSP_BASE=pmsis_bsp
export PMSIS_API_BASE=pmsis_api

AT_MANUAL_NAME= autotiler.pdf
HM_MANUAL_NAME= sdk_manual.pdf
APP_MANUAL_NAME= applications.pdf
MB_MANUAL_NAME= mbed-os.pdf
FR_MANUAL_NAME= freertos.pdf
VP_MANUAL_NAME= gvsoc.pdf
BM_MANUAL_NAME= benchmarks.pdf
PMSIS_BSP_MANUAL_NAME= pmsis-bsp.pdf
PMSIS_API_MANUAL_NAME= pmsis-api.pdf

HM_MANUAL_BITS= $(HM_BASE)/header.md $(HM_BASE)/readme_extract.md $(HM_BASE)/footer.md

APP_MANUAL_BITS= $(wildcard $(APP_BASE)/doc_*.md)
RT_MANUAL_BITS= $(addprefix $(RT_BASE)/, doc_sdk_manual.md doc_optimization.md doc_optimization_example.md)

ALL_TABS= HOME PULP-OS MBED-OS FREERTOS AUTOTILER APPLICATIONS GVSOC BENCHMARKS PMSIS_API PMSIS_BSP
ALL_PDFS= HOME_PDF AUTOTILER_PDF APPLICATIONS_PDF

all: reference

reference: $(ALL_TABS) $(HOME_PAGE)

pdf: reference $(ALL_PDFS)

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

$(TABSCRIPT_DST): $(TABSCRIPT_SRC) $(PATCHJS) $(BUILD_DIR)
	$(CAT) $< | $(PATCHJS) $(ALL_TABS) > $@

# first parameter is tab name i.e. AUTOTILER second is variable short name prefix i.e. AT
# third parameter is optional doxy build dependencies i.e. $(AT_BASE)/groups.h

define builddocs

$(2)_BUILD_ELEMS= $(BUILD_DIR)/$(1)/html
# .e.g. autotiler/images has to exist to build images and css for images (otherwise it is empty)
$(2)_HAS_IMAGES= $$(wildcard $$($(2)_BASE)/images)

ifneq ($$($(2)_HAS_IMAGES),)
  $(2)_IMAGES_DST_DIR=$(BUILD_DIR)/$(1)/html/images
  $(2)_IMAGES_SRC= $$(shell find $$($(2)_BASE)/images -name '*.png')
  $(2)_IMAGES_DST= $$(addprefix $$($(2)_IMAGES_DST_DIR)/, $$(notdir $$($(2)_IMAGES_SRC)))
  $(2)_BUILD_ELEMS+= $$($(2)_IMAGES_DST)
endif

$(1):	$$($(2)_BUILD_ELEMS)

$(BUILD_DIR)/$(1): $(BUILD_DIR)
	$(MKDIR) $$@

ifneq ($$($(2)_HAS_IMAGES),)
$(BUILD_DIR)/$(1)/images.css: $$($(2)_BASE)/mainpage.md $$(BUILD_DIR)/$(1)
	$(CAT) $$< | $(GENSTYLE) > $$@
else
$(BUILD_DIR)/$(1)/images.css: $$($(2)_BASE)/mainpage.md $$(BUILD_DIR)/$(1)
	echo "" > $$@
endif

$(BUILD_DIR)/$(1)/mainpage.md: $$($(2)_BASE)/doxy_header.md $$($(2)_BASE)/mainpage.md $(MDPD2DOXY) $$(BUILD_DIR)/$(1)
	$(CAT) $$($(2)_BASE)/doxy_header.md > $$@
	$(CAT) $$($(2)_BASE)/mainpage.md | $$(MDPD2DOXY) >> $$@

$(BUILD_DIR)/$(1)/html/images.css: $(BUILD_DIR)/$(1)/images.css
	$(CP) $(BUILD_DIR)/$(1)/images.css $(BUILD_DIR)/$(1)/html

ifneq ($$($(2)_HAS_IMAGES),)
$$($(2)_IMAGES_DST): $$($(2)_IMAGES_SRC) $$($(2)_IMAGES_DST_DIR)
	$(CP) $$($(2)_IMAGES_SRC) $$($(2)_IMAGES_DST_DIR)

$$($(2)_IMAGES_DST_DIR): $(BUILD_DIR)/$(1)/html
	$(MKDIR) $$@
endif

$(BUILD_DIR)/$(1)/html: $$($(2)_BASE)/doc.dxy $(TABSCRIPT_DST) $$(BUILD_DIR) $$(COMMON) $(3) $$(BUILD_DIR)/$(1)/mainpage.md $$(BUILD_DIR)/$(1)/images.css
	doxygen $$<

endef

$(eval $(call builddocs,HOME,HM))
$(eval $(call builddocs,AUTOTILER,AT,$(AT_BASE)/groups.h))
$(eval $(call builddocs,PULP-OS,RT,$(RT_BASE)/groups.h $(RT_BASE)/builtins.h $(GAP8_BUILTINS)))
$(eval $(call builddocs,APPLICATIONS,APP))
$(eval $(call builddocs,MBED-OS,MB,$(MB_BASE)/groups.h))
$(eval $(call builddocs,FREERTOS,FR,$(FR_BASE)/groups.h))
$(eval $(call builddocs,GVSOC,VP))
$(eval $(call builddocs,BENCHMARKS,BM))
$(eval $(call builddocs,PMSIS_BSP,PMSIS_BSP))
$(eval $(call builddocs,PMSIS_API,PMSIS_API))

define buildpdf

ifneq ($$($(3)_MANUAL_NAME),)
  $(3)_BUILD_ELEMS+= $(BUILD_DIR)/$$($(3)_MANUAL_NAME)
endif

# .e.g. AT_MANUAL_NAME needs to be set to build manual
ifneq ($$($(3)_MANUAL_NAME),)
$(BUILD_DIR)/$(2)/manual.md: $$($(3)_BASE)/pandoc_header.md $$($(3)_BASE)/mainpage.md
	$(CAT) $$($(3)_BASE)/pandoc_header.md $$($(3)_BASE)/mainpage.md $(4) >> $$@

$(BUILD_DIR)/$$($(3)_MANUAL_NAME): $(BUILD_DIR)/$(2)/manual.md
	$(CD) $$($(3)_BASE) && $(CAT) pandoc_header.md mainpage.md $(4) | $(PANDOC) -o $(DOC_DIR)/$$($(3)_MANUAL_NAME)
endif

$(1):$(BUILD_DIR)/$$($(3)_MANUAL_NAME)

endef

$(eval $(call buildpdf,HOME_PDF,HOME,HM,$(DOC_DIR)/$(RT_BASE)/mainpage.md))
$(eval $(call buildpdf,AUTOTILER_PDF,AUTOTILER,AT))
$(eval $(call buildpdf,APPLICATIONS_PDF,APPLICATIONS,APP))

# extra home build step
$(HM_BASE)/readme_extract.md: $(GAP_SDK_HOME)/README.md $(HM_BASE)/readme_extract.sh
	$(HM_BASE)/readme_extract.sh $@

$(HM_BASE)/mainpage.md: $(HM_MANUAL_BITS)
	$(RM) $@
	$(CAT) $(HM_MANUAL_BITS) >> $@

# extra applications build step
$(APP_BASE)/mainpage.md: $(APP_MANUAL_BITS)
	$(RM) $@
	$(CAT) $(APP_MANUAL_BITS) >> $@

# extra runtime build step
$(RT_BASE)/mainpage.md: $(RT_MANUAL_BITS)
	$(RM) $@
	$(CAT) $(RT_MANUAL_BITS) >> $@

define build_builtins
$(BUILD_DIR)/$(1): $(BUILTINS_DIR)/$(1) $(RT_BASE)/$(1).header $(RT_BASE)/$(1).footer $(BUILD_DIR)
	$(RM) $$@
	$(CAT) $(RT_BASE)/$(1).header $$< $(RT_BASE)/$(1).footer >> $$@
endef

# $(foreach H, $(BUILTINS_HEADERS), $(info $(call build_builtins,$(H))))
$(foreach H, $(BUILTINS_HEADERS), $(eval $(call build_builtins,$(H))))

# root index

$(HOME_PAGE): $(COM_BASE)/index.html $(BUILD_DIR)
	$(CP) $< $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(DOC_DIR)/*.pdf $(HOME_PAGE)
	$(RM) $(HM_BASE)/mainpage.md
	$(RM) $(HM_BASE)/readme_extract.md
	$(RM) $(APP_BASE)/mainpage.md
	$(RM) $(RT_BASE)/mainpage.md

upload: all
	lftp -c "open $(WWW_URL); mirror -RenL BUILD manuals/BUILD"

wwwlist:
	lftp -c "open $(WWW_URL); ls manuals/BUILD"

wwwclean:
	lftp -c "open $(WWW_URL); rm -r manuals/BUILD"

.PHONY: all reference clean $(ALL_TABS) pdf upload wwwlist wwwclean
