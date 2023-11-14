bittest = executable('test_bitreader', 'test_bitreader.c', include_directories : inc,
		     link_with : lib)
test('bitreader', bittest)

libtest = executable('test_libdeds', 'test_libdeds.c', include_directories : inc, link_with : lib)
test('decompress', libtest)
