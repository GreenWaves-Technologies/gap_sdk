SHELL=bash

PLATFORM ?= gvsoc
export PMSIS_OS ?= pulpos

ifdef PLATFORM
export PLPTEST_PLATFORM=$(PLATFORM)
endif

ifdef FULL_PLOT
export FULL_PLOT
endif

test:
	plptest --bench-csv-file=results.csv --testset=testset.cfg --stdout

score:
	plptest --bench-csv-file=results.csv --testset=testset.cfg score