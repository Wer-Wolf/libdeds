bittest = executable('test_bitreader', 'test_bitreader.c', include_directories : inc,
		     link_with : lib)
test('bitreader', bittest)

libtest = executable('test_libds', 'test_libds.c', include_directories : inc, link_with : lib)
test('decompress', libtest)
