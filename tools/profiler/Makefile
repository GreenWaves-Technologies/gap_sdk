.PHONY: all clean gui backend run docs

all: backend gui

backend:
	$(MAKE) -C backend

gui: backend
	cd gui && qmake && cd ..
	$(MAKE) -C gui
	rm -f gui/uic_wrapper.sh


clean:
	$(MAKE) -C backend clean
	if [ -e gui/Makefile]; then $(MAKE) -C gui clean; fi
	rm -rf docs

run: all
	bash ./run_profiler.sh $(folder)

docs:
	doxygen Doxyfile
