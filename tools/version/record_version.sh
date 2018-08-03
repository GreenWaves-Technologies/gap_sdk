#!/bin/bash

# Verison file PATH
VERSION_FILE_PATH="$(pwd)/version.log"

record_version() {
    cd    $1;

    # Record project path
    echo  "[$1]"                >> $VERSION_FILE_PATH

    # Record current commit number
    echo  "[commit]"                >> $VERSION_FILE_PATH
    git rev-parse --verify HEAD >> $VERSION_FILE_PATH

    echo ""                     >> $VERSION_FILE_PATH

    # Record status
    git status                  >> $VERSION_FILE_PATH
    echo ""                     >> $VERSION_FILE_PATH
    echo ""                     >> $VERSION_FILE_PATH
}

# Initialize the version file
echo ""                         > $VERSION_FILE_PATH

# GAP_SDK version
record_version $GAP_SDK_HOME;

# MBED-OS version
record_version $GAP_SDK_HOME/mbed-os;

# TF to GAP8 version
record_version $GAP_SDK_HOME/tf2gap8;

# TF to GAP8 version
record_version $GAP_SDK_HOME/applications;
