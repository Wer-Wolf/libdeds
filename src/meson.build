pkg = import('pkgconfig')
lib = library('libds', ['libds.c', 'bitreader.c'], include_directories : inc, install : true)
pkg.generate(lib)
