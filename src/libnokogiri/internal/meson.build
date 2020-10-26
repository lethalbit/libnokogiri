libnokogiri_headers_internal = files([
	'defs.hh',
	'fs.hh',
	'mmap.hh',
	'fd.hh',
])

if get_option('compressed_pcap_support')
	libnokogiri_headers_internal += files('zlib.hh')
	libnokogiri_srcs += files('zlib.cc')
endif

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_internal,
		subdir: 'libnokogiri/internal'
	)
endif
