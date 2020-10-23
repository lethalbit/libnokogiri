
# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'libnokogiri'
copyright = '2020, Aki Van Ness'
author = 'Aki Van Ness'

release = '0.0.1 3648191'


# -- General configuration ---------------------------------------------------
extensions = [
	'sphinx.ext.githubpages',
	'sphinx.ext.graphviz',
	'breathe',
	'exhale'
]

templates_path = ['/home/runner/work/libnokogiri/libnokogiri/docs/_templates']

source_suffix = {
	'.rst': 'restructuredtext',
	'.md': 'markdown',
}



exclude_patterns = [
	'meson.build',
	'config.py.in',
	'config.py',
	'Doxyfile.in',
	'Doxyfile'
]

breathe_projects = {
	project: '/home/runner/work/libnokogiri/libnokogiri/build/docs/doxyoutput/xml',
}
breathe_default_project = project

exhale_args = {
	'containmentFolder': '/home/runner/work/libnokogiri/libnokogiri/docs/api',
	'rootFileName': 'index.rst',
	'rootFileTitle': 'libnokogiri API',
	'doxygenStripFromPath': '/home/runner/work/libnokogiri/libnokogiri/src/libnokogiri',
	'createTreeView': True,
	'exhaleExecutesDoxygen': True,
	'exhaleDoxygenStdin': '''
	INPUT = /home/runner/work/libnokogiri/libnokogiri/src/libnokogiri
	EXCLUDE = /home/runner/work/libnokogiri/libnokogiri/src/libnokogiri/internal
	MACRO_EXPANSION = YES
	'''
}

primary_domain = 'cpp'
highlight_language = 'cpp'

graphviz_output_format = 'svg'

# -- Options for HTML output -------------------------------------------------
html_theme = 'alabaster'
html_static_path = ['/home/runner/work/libnokogiri/libnokogiri/docs/_static']
html_sidebars = { '**': ['about.html', 'relations.html', 'navigation.html'] }
html_theme_options = {
	'github_user': 'lethalbit',
	'github_repo': 'libnokogiri',
	'github_button': True,
	'fixed_sidebar': True,
}

# -- Options for cpp domain --------------------------------------------------
cpp_paren_attributes = [
	'LIBNOKOGIRI_NOWARN_UNUSED',
	'LIBNOKOGIRI_NO_DISCARD',
	'LIBNOKOGIRI_ALIGN',
	'LIBNOKOGIRI_DEPRECATE_R'
]

cpp_id_attributes = [
	'LIBNOKOGIRI_DEPRECATE',
]
