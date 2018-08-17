#!/bin/bash

# Verison file PATH
VERSION_FILE_PATH="$(pwd)/version.log"

record_version() {
    cd    $1;

    # Record project path
    echo "./gap_sdk${1#$GAP_SDK_HOME}"       >> $VERSION_FILE_PATH

    # Record current commit number
    echo  "[commit]"                         >> $VERSION_FILE_PATH
    git rev-parse --verify HEAD 2> /dev/null >> $VERSION_FILE_PATH

    echo ""                                  >> $VERSION_FILE_PATH

    # Record status
    git status                  2> /dev/null >> $VERSION_FILE_PATH
    echo ""                                  >> $VERSION_FILE_PATH
    echo ""                                  >> $VERSION_FILE_PATH
}

# Initialize the version file
echo ""                         > $VERSION_FILE_PATH

# GAP_SDK version
record_version $GAP_SDK_HOME;

# MBED-OS version
record_version $GAP_SDK_HOME/mbed-os;

# FreeRTOS version
record_version $GAP_SDK_HOME/freeRTOS;

# PULP-OS version
record_version $GAP_SDK_HOME/pulp-os;

# Examples version
record_version $GAP_SDK_HOME/examples;

# Mbed Examples version
record_version $GAP_SDK_HOME/examples/mbed-examples;

# FreeRTOS Examples version
record_version $GAP_SDK_HOME/examples/freeRTOS-examples;

# PULP Examples version
record_version $GAP_SDK_HOME/examples/pulp-examples;

# TF to GAP8 version
record_version $GAP_SDK_HOME/tf2gap8;

# Flasher version
record_version $GAP_SDK_HOME/tools/gap_flasher

# Application version
record_version $GAP_SDK_HOME/applications;

# Debug bridge version
record_version $GAP_SDK_HOME/tools/pulp-debug-bridge/
