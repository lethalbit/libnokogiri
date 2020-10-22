// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng/blocks/section_header.hh - pcapng section header block */
#if !defined(LIBNOKOGIRI_PCAPNG_BLOCKS_INTERFACE_DESCRIPTION_HH)
#define LIBNOKOGIRI_PCAPNG_BLOCKS_INTERFACE_DESCRIPTION_HH

#include <cstdint>

#include <libnokogiri/pcapng/block.hh>

namespace libnokogiri::pcapng::blocks {
	/*! \struct libnokogiri::pcapng::blocks::interface_description_t
		\brief A block that describes the interface on which the data was captured.


	*/
	struct interface_description_t final : public block_t {
	public:
		link_type_t _link_type;
		std::array<uint8_t, 16> _reserved;
		std::uint32_t _snap_len;
	};
}

#endif /* LIBNOKOGIRI_PCAPNG_BLOCKS_INTERFACE_DESCRIPTION_HH */
