#!/bin/sh

# Extract relevant bits of README.md to build into home page of docs

heading()
{
	echo "\\\newpage
# ${1}"
}

extract()
{
	cat ${GAP_SDK_HOME}/README.md | sed -n "/^${1}/,/^${2}/ {/^${1}\|${2}/!p}"
}

heading "Introduction to the GAP SDK" > ${1}
extract "## About" "##  Getting started" >> ${1}
heading "Installing the GAP SDK" >> ${1}
extract "##  Getting started" "## Getting help" >> ${1}
