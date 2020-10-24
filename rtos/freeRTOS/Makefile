DEFAULT_EXCLUDES  = --exclude=".*" \
                    --exclude="BUILD*" \
                    --exclude="__pycache__/" \
                    --exclude="junit-reports" \
                    --exclude="*.debug"
PRINTF_EXCLUDES   = --exclude="vendors/gwt/TARGET_GWT/libs/*printf"
PMSIS_EXCLUDES    = --exclude="vendors/gwt/TARGET_GWT/pmsis/api/docs"
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/bsp/Makefile"
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/bsp/docs"
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/bsp/transport/nina_w10/firmware"
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/implem/Makefile"

# Remove PMSIS_API and PMSIS_BSP to build artifact
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/api/"
PMSIS_EXCLUDES   += --exclude="vendors/gwt/TARGET_GWT/pmsis/bsp/"
#TRACKED_FILES    += vendors/gwt/TARGET_GWT/pmsis/api/*
#TRACKED_FILES    += vendors/gwt/TARGET_GWT/pmsis/bsp/*

# Remove GAP9 parts.
ifeq ($(GAP9), )
EXCLUDE_GAP9      = --exclude="freertos_kernel/portable/GCC/RI5CY-GAP9/"
EXCLUDE_GAP9     += --exclude="vendors/gwt/TARGET_GWT/TARGET_GAP9/"
EXCLUDE_GAP9     += --exclude="vendors/gwt/TARGET_GWT/pmsis/implem/pmsis_implem_gap9/"
EXCLUDE_GAP9     += --exclude="demos/gwt/gap9/"
else
EXCLUDE_GAP9      =
endif				# GAP9

#TRACKED_FILES=$(shell git ls-files --exclude-standard)
TRACKED_FILES     = .
TRACKED_FILES    += vendors/gwt/TARGET_GWT/libs/printf/printf.*
TRACKED_FILES    += vendors/gwt/TARGET_GWT/pmsis/implem/*

UNTRACKED_FILES   = $(shell git ls-files --exclude-standard --others)
EXCLUDE_UNTRACKED = $(foreach f, $(UNTRACKED_FILES), --exclude="$f")
ARTIFACT_EXCLUDES = $(DEFAULT_EXCLUDES) $(PRINTF_EXCLUDES) $(PMSIS_EXCLUDES) \
                    $(EXCLUDE_UNTRACKED) --exclude="/Makefile" \
                    $(EXCLUDE_GAP9)

artifact:
	@mkdir -p $(FREERTOS_ARTIFACT_PATH)
	@rsync -avR $(ARTIFACT_EXCLUDES) $(TRACKED_FILES) $(FREERTOS_ARTIFACT_PATH)

clean_artifact:
	@rm -rf $(FREERTOS_ARTIFACT_PATH)
