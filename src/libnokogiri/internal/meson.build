libnokogiri_headers_internal = files([
	'defs.hh',
	'fs.hh',
	'mmap.hh',
	'fd.hh',
	'zlib.hh',
	'file_wrapper.hh',
])

libnokogiri_srcs += files([
	'file_wrapper.cc'
])

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_internal,
		subdir: 'libnokogiri/internal'
	)
endif
