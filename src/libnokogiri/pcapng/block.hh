// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng/block.hh - Base type for pcapng blocks */
#if !defined(LIBNOKOGIRI_PCAPNG_BLOCK_HH)
#define LIBNOKOGIRI_PCAPNG_BLOCK_HH

#include <cstdint>
#include <utility>
#include <optional>

namespace libnokogiri::pcapng {
	/*! \enum libnokogiri::pcapng::block_type_t
		\brief Predefined block types

		The following block types are predefined by the pcapng specification, as
		such they are explicitly named.

		For documentation on how to register new block types with libnokogiri see
		the `Adding Blocks` section in `Extending libnokogiri`.
	*/
	enum struct block_type_t : std::uint32_t {
		Reserved                   = 0x00000000U,
		InterfaceDescription       = 0x00000001U, /*!< RFC Section 4.2 */
		Packet                     = 0x00000002U, /*!< RFC Appendix A  */
		SimplePacket               = 0x00000003U, /*!< RFC Section 4.4 */
		NameResolution             = 0x00000004U, /*!< RFC Section 4.4 */
		InterfaceStatistics        = 0x00000005U, /*!< RFC Section 4.6 */
		EnhancedPacket             = 0x00000006U, /*!< RFC Section 4.3 */
		SocketAggrigation          = 0x00000007U, /*!< Also used for IRIG Timestamps sometimes? */
		ARINC429AFDXEncapsulation  = 0x00000008U,
		SystemdJournalExport       = 0x00000009U, /*!< RFC Section 4.7 */
		DecryptionSecrets          = 0x0000000AU, /*!< RFC Section 4.8 */
		/* 0x0000000BU - 0x00000100U : Unallocated, presumed reserved */
		HoneProjectMachineInfo     = 0x00000101U,
		HoneProjectConnectionEvent = 0x00000102U,
		/* 0x00000103U - 0x00000200U : Unallocated, presumed reserved */
		SysdigMachineInfo          = 0x00000201U,
		SysdigProcessInfoV1        = 0x00000202U,
		SysdigFDList               = 0x00000203U,
		SysdigEvent                = 0x00000204U,
		SysdigInterfaceList        = 0x00000205U,
		SysdigUserList             = 0x00000206U,
		SysdigProcessInfoV2        = 0x00000207U,
		SysdigEventWithFlags       = 0x00000208U,
		SysdigProcessInfoV3        = 0x00000209U,
		SysdigProcessInfoV4        = 0x00000210U,
		SysdigProcessInfoV5        = 0x00000211U,
		SysdigProcessInfoV6        = 0x00000212U,
		SysdigProcessInfoV7        = 0x00000213U,
		/* 0x00000213U - 0x00000BACU : Unallocated, presumed reserved */
		CustomRewriteCopyable      = 0x00000BADU, /*!< RFC Section 4.9 */
		/* 0x00000BAEU - 0x40000BACU : Unallocated, presumed reserved */
		CustomRewriteUncopyable    = 0x40000BADU, /*!< RFC Section 4.9 */
		/* 0x40000BAEU - 0x0A0D0D09U : Unallocated, presumed reserved */
		SectionHeader              = 0x0A0D0D0AU, /* RFC Section 4.1 */
		/* 0x0A0D0D0BU - 0x0A0D09FFU : Unallocated, presumed reserved */
		/* 0x0A0D0A00U - 0x0A0D0AFFU : Reserved, Safegaurd range */
		/* 0x80000000U - 0xFFFFFFFFU : Reserved for local use */

	};

	/*! \struct libnokogiri::pcapng::block_t
		\brief Base for all pcapng blocks

		This structure represents the atomic base for all blocks defined by libnokogiri
		as well as by the developer if they add custom blocks.

		For documentation on how to add new blocks to libnokogiri see
		the `Adding Blocks` section in `Extending libnokogiri`

		Block have the following overarching structure

		```
		 0               1               2               3
		 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                          Block Type                           |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                       Total Block Size                        |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		/                          Block Data                           /
		/                Variable Length padded to 32 bits              /
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                       Total Block Size                        |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```

		- `Block Type` is a 32-bit unsigned value that indicates the type of block.
		- `Total Block Size` is a 32-bit unsigned value that indicates the size of the block
		- `Block Data` is the actual data for the block, its length is `Total Block Size` - 12

		The reason for the trailing `Total Block Size` field is to allow for bidirectional seeking
		in the pcap file.

		All blocks share this format.

		**NOTE:** The most significant bit (MSB) of the `Block Type` field indicates if the block if reserved
		for local use. An MSB of `0b1` indicates that it is a local block, where as an MSB of `0b0` indicates
		that the type is standard.

	*/
	struct block_t {
	private:
		block_type_t _type;
	protected:
		constexpr block_t(block_type_t type) noexcept :
			_type{type}
			{ /* NOP */ }
	public:
		constexpr block_t() noexcept :
			_type{block_type_t::Reserved}
			{ /* NOP */ }
		/*! Gets the type of the block */
		[[nodiscard]]
		block_type_t type() const noexcept { return _type; }
	};

	/*! \struct libnokogiri::pcapng::block_storage_t
		\brief Cached block information for pcapng sections

		This structure stores a small amount of information pertaining to a block
		in a section of a pcapng file.

		It holds the block type, the block length, and the offset into the pcapng file
		the block is located at.

		This class lets us know where every block is and it's size without actually
		loading the blocks into memory. This makes reading a file and searching for blocks
		much faster and more memory efficient in exchange for a small time penalty when
		first reading the file.

	*/
	struct block_storage_t final {
	private:
		block_type_t _type;
		std::uint32_t _length;
		std::uintptr_t _offset;
		std::optional<block_t> _block_cache;
	public:
		constexpr block_storage_t() noexcept :
			_type{block_type_t::Reserved}, _length{0U}, _offset{0U}, _block_cache{std::nullopt}
			{ /* NOP */ }

		constexpr block_storage_t(block_type_t type, std::uint32_t length, std::uintptr_t offset, std::optional<block_t> block = std::nullopt) noexcept :
			_type{type}, _length{length}, _offset{offset}, _block_cache{block}
			{ /* NOP */ }

		/*! Gets the type of the block stored */
		[[nodiscard]]
		block_type_t type() const noexcept { return _type; }
		/*! Gets the length of the block stored */
		[[nodiscard]]
		std::uint32_t length() const noexcept { return _length; }
		/*! Gets the offset of the block into the pcap file */
		[[nodiscard]]
		std::uintptr_t offset() const noexcept { return _offset; }
		/*! Gets the block from the file */
		[[nodiscard]]
		std::optional<block_t> get_block() noexcept {
			if (_block_cache == std::nullopt) {
				return std::nullopt;
			} else {
				return _block_cache;
			}
		}

	};
}

#endif /* LIBNOKOGIRI_PCAPNG_BLOCK_HH */
