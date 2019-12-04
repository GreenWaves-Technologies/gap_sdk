DEFAULT_EXCLUDES  = --exclude=".git*" \
                    --exclude="BUILD*" \
                    --exclude="__pycache__/" \
                    --exclude="junit-reports"
PRINTF_EXCLUDES   = --exclude="vendors/gwt/TARGET_GWT/libs/*printf"
PRINTF_EXCLUDES  +=
#TRACKED_FILES=$(shell git ls-files --exclude-standard)
TRACKED_FILES     = *
TRACKED_FILES    += vendors/gwt/TARGET_GWT/libs/printf/printf.*
UNTRACKED_FILES   = $(shell git ls-files --exclude-standard --others)
EXCLUDE_UNTRACKED = $(foreach f, $(UNTRACKED_FILES), --exclude="$f")
ARTIFACT_EXCLUDES = $(DEFAULT_EXCLUDES) $(PRINTF_EXCLUDES) \
                    $(EXCLUDE_UNTRACKED) \
                    --exclude="freertos_kernel/portable/GCC/RI5CY-GAP9"\
                    --exclude="vendors/gwt/TARGET_GWT/TARGET_GAP9/" \
                    --exclude=".*" \
                    --exclude="$(PWD)/Makefile"

artifact:
	@mkdir -p $(FREERTOS_ARTIFACT_PATH)
	@rsync -avR $(ARTIFACT_EXCLUDES) $(TRACKED_FILES:Makefile=) $(FREERTOS_ARTIFACT_PATH)

clean_artifact:
	@rm -rf $(FREERTOS_ARTIFACT_PATH)
