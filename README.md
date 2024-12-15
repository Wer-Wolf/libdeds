# libdeds - library for DoubleSpace decompression

This is a library implementing the DoubleSpace decompression algorithm.
Its mainly useful nowadays for decompressing Binary MOF (bmof) files
used within WMI (Windows Management Instrumentation).

The library focuses on readability instead of speed, to do not expect
overly high performance. The code itself is based on the code used within
the DMSDOS kernel module.

## Building

To build the library enter the following commands:

    meson setup build
    cd build
    meson compile

## Installation

To install this library, enter the following command:

    sudo meson install

Keep in mind that when using the ``ctypes`` python module the library might not
be found because it might end up being installed in ``/usr/local/lib/x86_64-linux-gnu``.

You can override this default setting by passing the options ``-Dlibdir=/usr/lib/x86_64-linux-gnu``
and ``-Dincludedir=/usr/include`` when building the library.

