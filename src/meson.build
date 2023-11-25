pkg = import('pkgconfig')
lib = library('deds', ['libdeds.c', 'bitreader.c'], include_directories : inc, install : true)
pkg.generate(lib)
