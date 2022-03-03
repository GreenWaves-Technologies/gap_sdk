# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

import sphinx_rtd_theme
import subprocess
import sys

def configure_doxyfile(file_in, file_out, replace_dict):
    with open(file_in, 'r') as file :
        filedata = file.read()

    for key, value in replace_dict.items():
        print(key, value)
        filedata = filedata.replace(key, value)

    with open(file_out, 'w') as file:
        file.write(filedata)

reference_input_dirs = [
        "../rtos/pmsis/pmsis_api/include/pmsis/chips/gap8/",
        "../rtos/pmsis/pmsis_api/include/pmsis/drivers/",
        "../rtos/pmsis/pmsis_api/include/pmsis/rtos/",
        "../rtos/pmsis/pmsis_api/include/pmsis/cluster/",
        "../rtos/pmsis/pmsis_api/include/pmsis/platforms/",
        "../rtos/pmsis/pmsis_api/include/pmsis/",
        "../rtos/pmsis/pmsis_bsp/include/",
        "source/reference/builtins/headers/",
    ]

replaces = {}
replaces["@DOXYGEN_OUTPUT_DIR@"] = "_build"
replaces["@DOXYGEN_INPUT_DIRS@"] = " \ \n".join(reference_input_dirs)

configure_doxyfile("gap_sdk.doxyfile.in", "gap_sdk.doxyfile", replaces)

try:
    subprocess.run("doxygen gap_sdk.doxyfile", shell=True, check=True)
except subprocess.CalledProcessError as e:
    print("Doxygen execution failed: ", e)
    sys.exit(1)

# -- Project information -----------------------------------------------------

project = 'GAP SDK'
copyright = '2021, GreenWaves Technologies'
author = 'GreenWaves Technologies'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
        "sphinx_rtd_theme",
        "breathe",
        "sphinx.ext.autodoc",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "sphinx_rtd_theme"
html_logo = "_static/logo.png"

html_theme_options = {
        'navigation_depth' : -1,
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# If true, "Created using Sphinx" is shown in the HTML footer. Default is True.
html_show_sphinx = False

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# Breathe configuration
breathe_projects = {
            "gap_sdk": "_build/xml",
        }

breathe_default_project = "gap_sdk"

breathe_domain_by_extension = {
    "h": "c",
    "c": "c",
}
breathe_separate_member_pages = True
breathe_show_enumvalue_initializer = True

# avoid warning due to unrecognized attribute
c_id_attributes = [
    'PI_INLINE_HYPER_LVL_0',
    'PI_INLINE_OCTOSPI_LVL_0',
    'PI_INLINE_CL_TEAM_0',
]

def setup(app):
    app.add_css_file("gap_sdk-custom.css")
