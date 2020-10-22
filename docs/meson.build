sphinx_build = find_program('sphinx-build', required: true)
doxygen = find_program('doxygen', required: true)

doc_config = configuration_data()
doc_config.set('VERSION_NUMBER', meson.project_version())
if get_option('debug')
	git =  find_program('git', required: false, native: true)

	if git.found()
		git_hash = run_command(git, 'rev-parse', '--short', 'HEAD').stdout().strip()
		if run_command(git, 'diff', '--quiet').returncode() != 0
		        git_hash += '-dirty'
		endif
		doc_config.set('GIT_HASH', git_hash)
	endif
else
	doc_config.set('GIT_HASH', '')
endif

doc_config.set('DOC_DIR', '@0@/docs'.format(meson.build_root()))

doc_config.set('SRC_DIR', meson.source_root())
doc_config.set('BLD_DIR', meson.build_root())


sphinx_config = configure_file(
	configuration: doc_config,
	input: 'conf.py.in',
	output: 'conf.py'
)

run_target(
	'build_docs',
	command: [
		sphinx_build,
		'-c', '@0@/docs/'.format(meson.build_root()),
		'-b', 'html',
		'@0@/docs'.format(meson.source_root()),
		'@0@/docs/'.format(meson.build_root()),
	]
)

