.. _getting_started:

Getting Started
===============

Follow this guide to:

- Setup a command line development environment on Ubuntu
- Get the source code
- Build and run a helloworld


Install dependencies
--------------------

You'll need to install some host dependencies using your package manager:

.. code-block:: bash

    # Update your system
    sudo apt udpate
    sudo apt upgrade

    # Install dependencies
    sudo apt-get install -y build-essential \
        git \
        libftdi-dev \
        libftdi1 doxygen \
        python3-pip \
        libsdl2-dev \
        curl \
        wget \
        cmake \
        libusb-1.0-0-dev \
        scons \
        gtkwave \
        libsndfile1-dev \
        rsync \
        autoconf \
        automake \
        texinfo \
        libtool \
        pkg-config \
        libsdl2-ttf-dev \
        libjpeg-dev

Install the toolchain
---------------------

Now you'll clone and install the GAP RISCV toolchain:

.. code-block:: bash

    git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu_18.git
    cd gap_riscv_toolchain_ubuntu_18
    # Depending on where you want to install it, you may need to run this command with sudo
    ./install.sh

Get the GAP SDK
---------------

Clone the GAP SDK repository:

.. code-block:: bash

    git clone https://github.com/GreenWaves-Technologies/gap_sdk.git
    cd gap_sdk

Install Python dependencies
---------------------------

.. code-block:: bash

    pip3 install --user -r requirements.txt

Compile the SDK
---------------

.. code-block:: bash

    # choose your board interactively
    source sourceme.sh
    # or select a board from the configuration files
    source configs/gapuino_v3.sh

    # Compile the whole GAP SDK
    make sdk

Build and run a HelloWorld
--------------------------

Now it's time to test your installation:

.. code-block:: bash

    cd examples/pmsis/helloworld
    make all run platform=gvsoc

And you should see a HelloWorld message coming from all GAP cores.

Next Steps
----------

Here are some next steps for exploring the GAP SDK:

- Try other :ref:`examples`
- Learn about :ref:`application_development`
- Discover :ref:`tools` such as the Autotiler and NNTool
- Check out :ref:`user_dev_guides` for other additional information
