if get_option('enable_fuzzing')
	foreach fmt : [ 'pcapng', 'pcap', 'pcap-ns', 'pcapng-gz', 'pcap-gz', 'pcap-ns-gz' ]
		message('Enabling fuzz target for @0@'.format(fmt))
		fuzzer = executable(
			'@0@-fuzz-harness'.format(fmt),
			'fuzzer.cc',
			cpp_args: [
				'-D_fuzz_target_@0@'.format(fmt)
			],
			include_directories: [ libnokogiri_inc ],
			dependencies: [ libnokogiri_deps ],
			link_with: libnokogiri,
		)

		run_target('fuzz-@0@'.format(fmt),
			command: ['@0@/etc/fuzz.sh'.format(meson.source_root()), fuzzer.full_path(), fmt],
			depends: fuzzer
		)
		message('Added \'fuzz-@0@\' target'.format(fmt))
	endforeach
endif
