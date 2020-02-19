#RELEASE_BRANCH=origin/sdk_release
RELEASE_BRANCH=origin/master
DEFAULT_EXCLUDES=--exclude=".git*" \
                 --exclude="BUILD*" \
                 --exclude="__pycache__/" \
                 --exclude="junit-reports"
TRACKED_FILES=$(shell git ls-tree -r --name-only $(RELEASE_BRANCH))
UNTRACKED_FILES=$(shell git ls-files --exclude-standard --others)
EXCLUDE_UNTRACKED= $(foreach f, $(UNTRACKED_FILES), --exclude="$f")
ARTIFACT_EXCLUDES=--exclude=$(DEFAULT_EXCLUDES) \
                  --exclude=$(EXCLUDE_UNTRACKED) \
                  --exclude=".*" \
                  --exclude="$(PWD)/Makefile"

artifact:
	@mkdir -p $(PMSIS_DRIVER_ARTIFACT_PATH)
	@rsync -avR $(ARTIFACT_EXCLUDES) $(TRACKED_FILES:Makefile=) $(PMSIS_DRIVER_ARTIFACT_PATH)

clean_artifact:
	@rm -rf $(PMSIS_DRIVER_ARTIFACT_PATH)
