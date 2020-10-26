===========
libnokogiri
===========

libnokogiri is a library for reading and writing `pcap <https://wiki.wireshark.org/Development/LibpcapFileFormat>`_ and `pcapng <https://github.com/pcapng/pcapng>`_ files for C++. It is designed to be as small and as light weight as possible, along with having an easy to use interface.

libnokogiri does not support the parsing of any packet specific data, it only handles the raw file format. You either need handle that yourself, or use a more appropriate library such as `libpcap <https://github.com/the-tcpdump-group/libpcap>`_ which has support for such things.

Configuring and Building
========================
Prerequisites
-------------
To build libnokogiri, ensure you have the following build time dependencies:
 * git
 * meson
 * ninja
 * zlib >= 1.1.130
 * g++ >= 9 or clang++ >= 9

If you wish to build the documentation, the you will also need the following:
 * sphinx-build
 * python-breath
 * python-exhale
 * doxygen


Configuring
-----------

You can build Nox with the default options, all of which can be found in `meson_options.txt <https://github.com/lethalbit/libnokogiri/blob/main/meson_options.txt>`_. You can change these by specifying ``-D<OPTION_NAME>=<VALUE>`` at initial meson invocation time, or with `meson configure` in the build directory post initial configure.

To change the install prefix, which is ``/usr/local`` by default ensure to pass ``--prefix <PREFIX>`` when running meson for the first time.

In either case, simpling running `meson build` from the root of the repository will be sufficient and place all of the build files in the `build` subdirectory.

Building
--------
Once you have configured libnokogiri appropriately, to simply build and install simply run the following:::

	$ ninja -c build
	$ ninja -c build install


This will build and install libnokogiri into the default prefix which is `/usr/local`, to change that see the configuration steps above.

Notes to Package Maintainers
----------------------------
If you are building libnokogiri for inclusion in a distributions package system then ensure to set `DESTDIR` prior to running meson install.

There is also a `bugreport_url` configuration option that is set to this repositories issues tracker by default, it is recommended to change it to your distributions bug tracking page.

License
=======

libnokogiri is licensed under the LGPL v3 or later, you can find a full copy of the license text in the `LICENSE <https://github.com/lethalbit/libnokogiri/blob/main/LICENSE>`_ file .
