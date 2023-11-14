pkg = import('pkgconfig')
lib = library('libdeds', ['libdeds.c', 'bitreader.c'], include_directories : inc, install : true)
pkg.generate(lib)
