# Decompressor automatic tests

The goal of this test is to test the maximum amounts of cases for the decompressor.
To do so we generate various input data, and execute the test.

Be aware that running the full test is quite long (multiple hours on RTL simulation).

## How to launch

`./test.pl` will launch the full automated test.

When a test is failing the automatic test will stop, and display the configuration that failed.

## Run example on board 
It is possible to run these tests on physical board by setting the platform to "board" in generate_data_and_test function.