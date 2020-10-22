// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng/blocks/section_header.hh - pcapng section header block */
#if !defined(LIBNOKOGIRI_PCAPNG_BLOCKS_SECTION_HEADER_HH)
#define LIBNOKOGIRI_PCAPNG_BLOCKS_SECTION_HEADER_HH

#include <cstdint>

#include <libnokogiri/common.hh>

#include <libnokogiri/pcapng/block.hh>

namespace libnokogiri::pcapng::blocks {
	/*! \struct libnokogiri::pcapng::blocks::section_header_t
		\brief Block that indicates the start of a section in a pcapng file

		The section header block is the only mandatory block type in pcapng files.
		It identifies the beginning of a section of the capture file.

		This block does not contain capture data itself but rather it identifies
		a collection of blocks that are logically correlated.

		There can be multiple of these blocks in the same pcapng file, however
		a section can not contain another section.
	*/
	struct section_header_t final : public block_t {
	public:
		constexpr static std::uint32_t magic = 0x1A2B3C4D;
	private:
		std::uint32_t _bom;
		version_t _version;
		std::int64_t _section_length;
		// std::vector<libnokogiri::pcapng::option_t> _options;
	public:
		constexpr section_header_t() noexcept :
			block_t(block_type_t::SectionHeader),
			_bom{section_header_t::magic}, _version{1U, 0U}, _section_length{-1} //, _options{}
			{ /* NOP */ }

		constexpr section_header_t(std::uint32_t bom, version_t version, std::int64_t length) noexcept :
			block_t(block_type_t::SectionHeader),
			_bom{bom}, _version{version}, _section_length{length} //, _options{}
			{ /* NOP */ }

		/*! Gets the byte-order-mark, used for checking the endian of the file */
		[[nodiscard]]
		std::uint32_t bom() const noexcept { return _bom; }
		/*! Gets the version of the section */
		[[nodiscard]]
		version_t version() const noexcept { return _version; }
		/*! Gets the length of the section, if -1 is returned the section length must be calculated by scanning every block and totaling the sizes */
		[[nodiscard]]
		std::int64_t section_length() const noexcept { return _section_length; }


	};
}

#endif /* LIBNOKOGIRI_PCAPNG_BLOCKS_SECTION_HEADER_HH */
