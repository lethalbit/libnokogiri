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

pcapng_test_files = files([
	'test_data/pcapng/file1.pcapng',
	'test_data/pcapng/file1.pcapng.gz',
	'test_data/pcapng/file2.pcapng',
	'test_data/pcapng/file2.pcapng.gz',
	'test_data/pcapng/file3.pcapng',
	'test_data/pcapng/file3.pcapng.gz',
	'test_data/pcapng/file4.pcapng',
	'test_data/pcapng/file4.pcapng.gz',
	'test_data/pcapng/file5.pcapng',
	'test_data/pcapng/file5.pcapng.gz',
	'test_data/pcapng/file6.pcapng',
	'test_data/pcapng/file6.pcapng.gz',
	'test_data/pcapng/file7.pcapng',
	'test_data/pcapng/file7.pcapng.gz',
	'test_data/pcapng/file8.pcapng',
	'test_data/pcapng/file8.pcapng.gz',
	'test_data/pcapng/file9.pcapng',
	'test_data/pcapng/file9.pcapng.gz',
	'test_data/pcapng/fileA.pcapng',
	'test_data/pcapng/fileA.pcapng.gz',
])

pcap_test_files = files([
	'test_data/pcap/file1.pcap',
	'test_data/pcap/file1.pcap.gz',
	'test_data/pcap/file2.pcap',
	'test_data/pcap/file2.pcap.gz',
	'test_data/pcap/file3.pcap',
	'test_data/pcap/file3.pcap.gz',
	'test_data/pcap/file4.pcap',
	'test_data/pcap/file4.pcap.gz',
	'test_data/pcap/file5.pcap',
	'test_data/pcap/file5.pcap.gz',
	'test_data/pcap/file6.pcap',
	'test_data/pcap/file6.pcap.gz',
	'test_data/pcap/file7.pcap',
	'test_data/pcap/file7.pcap.gz',
	'test_data/pcap/file8.pcap',
	'test_data/pcap/file8.pcap.gz',
	'test_data/pcap/file9.pcap',
	'test_data/pcap/file9.pcap.gz',
	'test_data/pcap/fileA.pcap',
	'test_data/pcap/fileA.pcap.gz',

	'test_data/pcap/file1.ns.pcap',
	'test_data/pcap/file1.ns.pcap.gz',
	'test_data/pcap/file2.ns.pcap',
	'test_data/pcap/file2.ns.pcap.gz',
	'test_data/pcap/file3.ns.pcap',
	'test_data/pcap/file3.ns.pcap.gz',
	'test_data/pcap/file4.ns.pcap',
	'test_data/pcap/file4.ns.pcap.gz',
	'test_data/pcap/file5.ns.pcap',
	'test_data/pcap/file5.ns.pcap.gz',
	'test_data/pcap/file6.ns.pcap',
	'test_data/pcap/file6.ns.pcap.gz',
	'test_data/pcap/file7.ns.pcap',
	'test_data/pcap/file7.ns.pcap.gz',
	'test_data/pcap/file8.ns.pcap',
	'test_data/pcap/file8.ns.pcap.gz',
	'test_data/pcap/file9.ns.pcap',
	'test_data/pcap/file9.ns.pcap.gz',
	'test_data/pcap/fileA.ns.pcap',
	'test_data/pcap/fileA.ns.pcap.gz',
])

pcapng_test_host = executable(
	'libnokogiri-pcapng-test',
	'pcapng_test.cc',
	include_directories: [ libnokogiri_inc ],
	dependencies: [ libnokogiri_deps ],
	link_with: libnokogiri,
)

pcap_test_host = executable(
	'libnokogiri-pcap-test',
	'pcap_test.cc',
	include_directories: [ libnokogiri_inc ],
	dependencies: [ libnokogiri_deps ],
	link_with: libnokogiri,
)

foreach f : pcapng_test_files
	test(
		'pcapng read test on "@0@"'.format(f),
		pcapng_test_host,
		args: [
			'-r',
			f,
		]
	)
endforeach

foreach f : pcap_test_files
	test(
		'pcap read test on "@0@"'.format(f),
		pcap_test_host,
		args: [
			'-r',
			f,
		]
	)
endforeach

foreach f : pcapng_test_files
	test(
		'pcapng write test on "@0@"'.format(f),
		pcapng_test_host,
		args: [
			'-w',
			f,
			meson.build_root(),
		]
	)
endforeach

foreach f : pcap_test_files
	test(
		'pcap write test on "@0@"'.format(f),
		pcap_test_host,
		args: [
			'-w',
			f,
			meson.build_root(),
		]
	)
endforeach

libnokogiri_unit_srcs = files([
	# Catch test runner
	'catch2/catch_amalgamated.cpp'
])

libnokogiri_unit_test = executable(
	'libnokogiri-unit-tests',
	libnokogiri_unit_srcs,
	include_directories: [ libnokogiri_inc, include_directories('catch2') ],
	dependencies: [ libnokogiri_deps ],
	link_with: libnokogiri,
)

test('libnokogiri catch tests', libnokogiri_unit_test)
