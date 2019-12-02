export SRC

all:
ifdef PULPOS
	make -f rules/pulpos.mk build install
endif

clean:
ifdef PULPOS
	make -f rules/pulpos.mk clean
endif

header:
ifdef PULPOS
	make -f rules/pulpos.mk header
endif