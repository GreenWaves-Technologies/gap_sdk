export chip=GAP8

rtl_list_tests:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh list rtl

gvsoc_list_tests:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh list gvsoc

gapuino_list_tests:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh list gapuino

rtl_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh run rtl

rtl_compile_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile rtl

rtl_compile_and_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile_and_run rtl

gvsoc_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh run gvsoc

gvsoc_compile_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile gvsoc

gvsoc_compile_and_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile_and_run gvsoc

gapuino_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh run gapuino

gapuino_compile_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile gapuino

gapuino_compile_and_run_test:
	$(GAP_SDK_HOME)/tools/runner/run_all_tests.sh compile_and_run gapuino

clean:
	@find . -name BUILD -exec rm -rf {} +
	@find . -name transcript -exec rm  {} +
	@find . -name version.log -exec rm {} +
	@rm -rf report.log
