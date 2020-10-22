// SPDX-License-Identifier: LGPL-3.0-or-later
/* libnokogiri.hh - libnokogiri "unity" include */
#if !defined(LIBNOKOGIRI_HH)
#define LIBNOKOGIRI_HH

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>
#include <libnokogiri/pcap.hh>
#include <libnokogiri/pcapng.hh>

/*! \namespace libnokogiri
	\brief Overarching libnokogiri namespace
*/
namespace libnokogiri {
	/*! \namespace libnokogiri::pcap
		\brief libnokogiri pcap interface

		All of the objects and machinery in order to read and write legacy pcap files
		are located in this namespace.
	*/
	namespace pcap {}
	/*! \namespace libnokogiri::pcapng
		\brief libnokogiri pcapng interface

		All of the objects and machinery in order to read and write pcapng files
		are located in this namespace.
	*/
	namespace pcapng {
		/*! \namespace libnokogiri::pcapng::blocks
			\brief pcapng blocks that are defined as per the standard

			This namespace contains all the pre-specified blocks as defined by the
			pcapng RFC.

			For documentation on how to add new blocks to libnokogiri see
			the `Adding Blocks` section in `Extending libnokogiri`
		*/
		namespace blocks {}

		/*! \namespace libnokogiri::pcapng::options
			\brief pcapng options that are defined as per the standard

			This namespace contains all the pre-specified options as defined by the
			pcapng RFC.

			For documentation on how to register new option types with libnokogiri see
			the `Adding Options` section in `Extending libnokogiri`.
		*/
		namespace options {}
	}
	/*! \namespace libnokogiri::compiletime
		\brief libnokogiri library configuration constants

		This namespace contains constants that were configured at compile time
		for the library, thus is indicates things like version number, zlib support
		and other miscellaneous things.
	*/
	namespace compiletime {}
}

#endif /* LIBNOKOGIRI_HH */
