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

import sphinx_rtd_theme
import subprocess
import sys

import os
# Add gvsoc_control python module
sys.path.insert(0, os.path.abspath('../gvsoc/gvsoc/engine/python/gv/'))

def generate_doxygen_documentation(project_name, exclude_dirs, include_dirs):
    def configure_doxyfile(file_in, file_out, replace_dict):
        with open(file_in, 'r') as file :
            filedata = file.read()

        for key, value in replace_dict.items():
            print(key, value)
            filedata = filedata.replace(key, value)

        with open(file_out, 'w') as file:
            file.write("###################################\n")
            file.write("### GENERATED FILE, DO NOT EDIT ###\n")
            file.write("###################################\n\n")
            file.write(filedata)

    # Configure the doxyfile
    replaces = {}
    replaces["@DOXYGEN_PROJECT_NAME@"] = project_name
    replaces["@DOXYGEN_OUTPUT_DIR@"] = "_build"
    replaces["@DOXYGEN_XML_DIR@"] = "xml_{}".format(project_name)
    replaces["@DOXYGEN_INPUT_DIRS@"] = " \ \n".join(include_dirs)
    replaces["@DOXYGEN_EXCLUDE_DIRS@"] = " \ \n".join(exclude_dirs)

    generated_doxyfile = "{}.doxyfile".format(project_name)
    configure_doxyfile("template.doxyfile.in",
                       generated_doxyfile,
                       replaces)
    # Run doxygen
    try:
        subprocess.run("doxygen {}".format(generated_doxyfile), shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print("Doxygen execution failed: ", e)
        sys.exit(1)

    return "{}/{}".format(replaces["@DOXYGEN_OUTPUT_DIR@"], replaces["@DOXYGEN_XML_DIR@"])

# -- Project information -----------------------------------------------------

project = 'GAP SDK'
copyright = '2022, GreenWaves Technologies'
author = 'GreenWaves Technologies'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
        "sphinx_rtd_theme",
        "breathe",
        "sphinx.ext.autodoc",
        "sphinx_tabs.tabs",
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

#########################
# Breathe configuration #
#########################

gap_sdk_reference_input_dirs = [
        "../rtos/pmsis/api/include/pmsis/drivers/",
        "../rtos/pmsis/api/include/pmsis/rtos/",
        "../rtos/pmsis/api/include/pmsis/cluster/",
        "../rtos/pmsis/api/include/pmsis/platforms/",
        "../rtos/pmsis/api/include/pmsis/",
        "../rtos/pmsis/bsp/include/",
        "../rtos/sfu/include/",
        "source/reference/builtins/headers/",
        "../tools/audio-framework/runtime/include/",
    ]

gap_sdk_exclude_dirs = [
        "*/vega/*",
        "*/gap8/*",
        "*/gap9/*",
    ]

gap8_reference_input_dirs = [
        "../rtos/pmsis/api/include/pmsis/chips/gap8/",
        "../rtos/pmsis/api/include/pmsis/drivers/",
    ]

gap8_exclude_dirs = [
        "*/vega/*",
        "*/gap9/*",
    ]

gap9_reference_input_dirs = [
        "../rtos/pmsis/api/include/pmsis/chips/gap9/",
    ]

gap9_exclude_dirs = [
        "*/vega/*",
        "*/gap8/*",
    ]

gap_sdk_xml_dir = generate_doxygen_documentation("gap_sdk", gap_sdk_exclude_dirs, gap_sdk_reference_input_dirs)
gap8_xml_dir    = generate_doxygen_documentation("gap8"   , gap8_exclude_dirs   , gap8_reference_input_dirs)
gap9_xml_dir    = generate_doxygen_documentation("gap9"   , gap9_exclude_dirs   , gap9_reference_input_dirs)

breathe_projects = {
            "gap_sdk": gap_sdk_xml_dir,
            "gap8": gap8_xml_dir,
            "gap9": gap9_xml_dir,
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
    'PI_INLINE_I2S_LVL_0',
    'PI_WATCHDOG_INLINE0',
]

#############################
# Sphinx tabs configuration #
#############################
sphinx_tabs_disable_tab_closing = True

def setup(app):
    app.add_css_file("gap_sdk-custom.css")
