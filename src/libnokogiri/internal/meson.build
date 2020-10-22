libnokogiri_headers_internal = files([
	'defs.hh',
	'fs.hh',
	'mmap.hh',
	'fd.hh',
])

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_internal,
		subdir: 'libnokogiri/internal'
	)
endif
