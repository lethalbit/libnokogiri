libnokogiri_headers_internal = files([
	'defs.hh',
	'fd.hh',
	'fs.hh',
	'zlib.hh',
])

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_internal,
		subdir: 'libnokogiri/internal'
	)
endif
