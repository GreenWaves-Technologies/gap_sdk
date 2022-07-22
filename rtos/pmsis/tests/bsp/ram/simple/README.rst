CMake how-to
============

Test Description:
-----------------

Run a ram transfer on FC.
Note: SPI is not supported on gap9 gvsoc as of now

Test options:
-------------

-DRAM_TYPER=[SPIRAM/HYPERRAM]: choose which ram type to use

Few command examples for:
-------------------------
- freertos
- SPI/Hyper ram


Generate build files (Ninja generator is optional - requires installation of ninja-build package):

.. code-block:: shell

    # without cluster:
    CMAKE_GENERATOR=Ninja cmake -S . -B build -DCONFIG_GAP_SDK_HOME=$GAP_SDK_HOME -DCMAKE_MODULE_PATH=$GAP_SDK_HOME/utils/cmake -DCONFIG_CHIP=GAP9 -DCONFIG_CHIP_VERSION=2 -DCONFIG_PMSIS_OS=freertos -DBOARD=$BOARD_NAME -DRAM_TYPE=SPIRAM
    # With cluster:
    CMAKE_GENERATOR=Ninja cmake -S . -B build -DCONFIG_GAP_SDK_HOME=$GAP_SDK_HOME -DCMAKE_MODULE_PATH=$GAP_SDK_HOME/utils/cmake -DCONFIG_CHIP=GAP9 -DCONFIG_CHIP_VERSION=2 -DCONFIG_PMSIS_OS=freertos -DBOARD=$BOARD_NAME -DRAM_TYPE=HYPERRAM

Build

.. code-block:: shell

    cmake --build build

Run

.. code-block:: shell

    # Without traces
    cmake --build build --target run
    # with fc traces (for other traces, read gvsoc documentation)
    runner_args="--trace=fc/insn"  cmake --build . --target run

Cleanup

.. code-block:: shell

    rm -rf build
