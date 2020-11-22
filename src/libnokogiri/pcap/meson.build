libnokogiri_headers_pcap = files([
	'header.hh',
	'packet.hh',
])

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_pcap,
		subdir: 'libnokogiri/pcap'
	)
endif
