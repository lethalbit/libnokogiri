project(
	'libnokogiri',
	'cpp',
	default_options: [
		'cpp_std=c++17',
		'warning_level=3'
	],
	version: '0.0.1',
	license: 'LGPL-3.0-or-later'
)

cxx = meson.get_compiler('cpp')

extra_args = [
	'-Wvla',
	'-Wimplicit-fallthrough'
]

add_project_arguments(
	cxx.get_supported_arguments(extra_args),
	language: 'cpp'
)

if target_machine.system() == 'windows'
	zlib = subproject('zlib')
	libnokogiri_deps = [
		zlib.get_variable('zlib_dep')
	]
else
	libnokogiri_deps = [
		dependency('zlib', version: '>=1.1.130', required: true)
	]
endif

subdir('src')

if get_option('build_docs')
	subdir('docs')
endif

if get_option('enable_tests') and not meson.is_subproject()
	subdir('tests')
endif

if get_option('build_examples')
	subdir('examples')
endif

if not meson.is_subproject() and target_machine.system() != 'windows'
	pkgconfig = import('pkgconfig')
	pkgconfig.generate(
		subdirs: '.',
		name: meson.project_name(),
		filebase: meson.project_name(),
		libraries: [ libnokogiri, libnokogiri_deps ],
		version: meson.project_version(),
		description: 'A small, easy to use library for reading and writing pcap and pcapng files'
	)
endif


libnokogiri_dep = declare_dependency(
	include_directories: [ libnokogiri_inc ],
	link_with: libnokogiri,
	dependencies: libnokogiri_deps,
	version: meson.project_version()
)
