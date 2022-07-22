CMake how-to
============

Test Description:
-----------------

Run an helloworld on FC and optionnaly cluster too

Test options:
-------------

-DCLUSTER=1: also run hello code on cluster

Few command examples for:
-------------------------
- gap9_v2
- freertos
- Without and with cluster


Generate build files (Ninja generator is optional - requires installation of ninja-build package):

.. code-block:: shell

    # without cluster:
    CMAKE_GENERATOR=Ninja cmake -S . -B build -DCONFIG_GAP_SDK_HOME=$GAP_SDK_HOME -DCMAKE_MODULE_PATH=$GAP_SDK_HOME/utils/cmake -DCONFIG_CHIP=GAP9 -DCONFIG_CHIP_VERSION=2 -DCONFIG_PMSIS_OS=freertos
    # With cluster:
    CMAKE_GENERATOR=Ninja cmake -S . -B build -DCONFIG_GAP_SDK_HOME=$GAP_SDK_HOME -DCMAKE_MODULE_PATH=$GAP_SDK_HOME/utils/cmake -DCONFIG_CHIP=GAP9 -DCONFIG_CHIP_VERSION=2 -DCONFIG_PMSIS_OS=freertos -DCLUSTER=1

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
