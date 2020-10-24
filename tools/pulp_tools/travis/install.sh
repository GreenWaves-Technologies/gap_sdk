if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    brew install libusb
    wget -O riscv-toolchain.tar.gz https://dl.bintray.com/platformio/dl-packages/toolchain-riscv-pulp-darwin_x86_64-1.70101.181014.tar.gz
else
    wget -O riscv-toolchain.tar.gz https://dl.bintray.com/platformio/dl-packages/toolchain-riscv-pulp-linux_x86_64-1.70101.181017.tar.gz
fi

mkdir riscv-toolchain && tar xvf riscv-toolchain.tar.gz -C riscv-toolchain
