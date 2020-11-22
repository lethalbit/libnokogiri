libnokogiri_headers_pcapng = files([
	'block.hh',
	'blocks.hh',
	'option.hh',
	'options.hh',
	'section.hh',
])

subdir('blocks')
subdir('options')

if not meson.is_subproject()
	install_headers(
		libnokogiri_headers_pcapng,
		subdir: 'libnokogiri/pcapng'
	)
endif
