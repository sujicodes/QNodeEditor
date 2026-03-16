# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'QNodeEditor'
copyright = '2026, Sujan Sureshan'
author = 'Sujan Sureshan'
release = 'V1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
]
 
breathe_projects = {
    "QNodeEditor": "../../doxygen/xml"
}
breathe_default_project = "QNodeEditor"
 
html_theme = 'sphinx_rtd_theme'

templates_path = ['_templates']
exclude_patterns = []

