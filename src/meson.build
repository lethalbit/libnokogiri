

libnokogiri_inc = [
	include_directories('.')
]

libnokogiri_srcs = [
	'libnokogiri.cc',
]

subdir('libnokogiri')

libnokogiri = library(
	'nokogiri',
    libnokogiri_srcs,
	gnu_symbol_visibility: 'inlineshidden',
	implicit_include_directories: false,
	pic: true,
    include_directories: libnokogiri_inc,
    dependencies: libnokogiri_deps,
	install: (not meson.is_subproject())
)
