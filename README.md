# libdeds - library for DoubleSpace decompression

This is a library implementing the DoubleSpace decompression algorithm.
Its mainly useful nowadays for decompressing Binary MOF (bmof) files
used within WMI (Windows Management Instrumentation).

The library focuses on readability instead of speed, to do not expect
overly high performance. The code itself is based on the code used within
the DMSDOS kernel module.

