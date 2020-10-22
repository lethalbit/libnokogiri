// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng/section.hh - Container for pcapng file sections */
#if !defined(LIBNOKOGIRI_PCAPNG_SECTION_HH)
#define LIBNOKOGIRI_PCAPNG_SECTION_HH

#include <cstdint>
#include <vector>

#include <libnokogiri/pcapng/block.hh>
#include <libnokogiri/pcapng/blocks/section_header.hh>

namespace libnokogiri::pcapng {
	/*! \struct libnokogiri::pcapng::section_t
		\brief Storage for sections within a pcapng file

	*/
	struct section_t final {
	private:
		std::uint32_t _length;
		std::uintptr_t _offset;
		blocks::section_header_t _header;
		std::vector<block_storage_t> _blocks;

	};
}

#endif /* LIBNOKOGIRI_PCAPNG_SECTION_HH */
