// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap/header.hh - libnokogiri pcap file header */
#if !defined(LIBNOKOGIRI_PCAP_HEADER_HH)
#define LIBNOKOGIRI_PCAP_HEADER_HH

#include <cstdint>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>

namespace libnokogiri::pcap {
	/*! \enum libnokogiri::pcap::pcap_variant_t
		\brief Types of possible pcap formats
	 */
	enum struct pcap_variant_t : std::uint32_t {
		Standard   = 0xA1B2C3D4U, /*! "Standard" pcap file magic  */
		Modified   = 0xA1B2CD34U, /*! Magic for the modified pcap format introduced by Alexey Kuznetsov's patches to some versions of libpcap */
		IXIAHW     = 0x1C0001ACU, /*! Magic for IXIA's lcap format */
		IXIASW     = 0x1C0001ABU, /*! Magic for IXIA's lcap format */
		Nanosecond = 0xA1B23C4DU, /*! Magic for the modified pcap format introduced by Ulf Lamping's patches. It's identical to the standard but the timestamps are in nanoseconds. */

		/* Swapped versions of the magic, so it's easy to detect */
		SwappedStantard   = 0xD4C3B2A1U, /*!< Swapped magic for standard pcap files */
		SwappedModified   = 0x34CDB2A1U, /*!< Swapped magic for modified pcap files */
		SwappedIXIAHW     = 0xAC01001CU, /*!< Swapped magic for IXIA's lcap */
		SwappedIXIASW     = 0xAB01001CU, /*!< Swapped magic for IXIA's lcap */
		SwappedNanosecond = 0x4D3CB2A1U, /*!< Swapped magic for nanosecond pcap files */
	};

	/*! \struct libnokogiri::pcap::file_header_t
		\brief pcap file header

		This is the header that is at the very beginning of a pcap file. It contains the magic numbers
		as well as various information related to the contents of the capture.

		It's organized as follows:

		```
		 0               1               2               3
		 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                        PCAP Magic Number                      |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|      Major Version Number     |      Minor Version Number     |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                     Timezone GMT Offset                       |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                      Time-stamp Accuracy                      |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                       Max Packet Length                       |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                        Data Link Type                         |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```

		 - `PCAP Magic Number` - The 32-bit unsigned value is used to identify the type of pcap file and its endianness.
		 - `Major Version Number` & `Minor Version Number` Two unsigned 16-bit values to indicate the version of the pcap
		 - `Timezone GMT Offset` - A signed 32-bit value which describes the timezone offset in seconds from GMT that this capture was taken at. (normally set to `0`)
		 - `Time-stamp Accuracy` - An unsigned 32-bit value describing the number of significant figures the timestamps have (normally `0`)
		 - `Max Packet Length` - An unsigned 32-bit value that represents the maximum number of octets saved per packet in this file. Packets larger than this value will be truncated.
		 - `Data Link Type` - Am unsigned 32-bit value describing the link layer. See libnokogiri::link_type_t for possible values.

		The most recent version for pcap is 2.4.

	*/
	struct file_header_t final {
	private:
		pcap_variant_t _variant;
		version_t _version;
		std::int32_t _tz_offset;
		std::uint32_t _timestamp_figs;
		std::uint32_t _pcklen_max;
		alignas(alignof(std::uint32_t)) link_type_t _network;
	public:
		constexpr file_header_t() noexcept :
			_variant{pcap_variant_t::Standard}, _version{1U, 0U}, _tz_offset{0},
			_timestamp_figs{0U}, _pcklen_max{0U}, _network{link_type_t::User0}
			{ /* NOP */ }

		constexpr file_header_t(pcap_variant_t variant, version_t version, std::int32_t tz_offset,
				std::uint32_t ts_figs, std::uint32_t pcklen, link_type_t network) noexcept :
			_variant{variant}, _version{version}, _tz_offset{tz_offset},
			_timestamp_figs{ts_figs}, _pcklen_max{pcklen}, _network{network}
			{ /* NOP */ }

		/*! Retrieve the type of pcap file this is. This is also the magic number for the file  */
		[[nodiscard]]
		pcap_variant_t variant() const noexcept { return _variant; }
		/*! Set the type of pcap file this is. This is also the magic number for the file  */
		void variant(const pcap_variant_t variant) noexcept { _variant = variant; }

		/*! Retrieve the pcap file version. */
		[[nodiscard]]
		version_t version() const noexcept { return _version; }
		/*! Set the pcap file version. */
		void version(const version_t version) noexcept { _version = version; }

		/*! Retrieve the offset for the timezone in seconds relative to GMT. */
		[[nodiscard]]
		std::int32_t timezone_offset() const noexcept { return _tz_offset; }
		/*! Set the offset for the timezone in seconds relative to GMT. */
		void timezone_offset(const std::int32_t tz_offset) noexcept { _tz_offset = tz_offset; }

		/*! Retrieve the accuracy of the timestamps */
		[[nodiscard]]
		std::uint32_t timestamp_accuracy() const noexcept { return _timestamp_figs; }
		/*! Set the accuracy of the timestamps */
		void timestamp_accuracy(const std::uint32_t ts_accuracy) noexcept { _timestamp_figs = ts_accuracy; }

		/*! Retrieve the maximum length a packet can be in this pcap file. */
		[[nodiscard]]
		std::uint32_t max_packet_length() const noexcept { return _pcklen_max; }
		/*! Set the maximum length a packet can be in this pcap file. */
		void max_packet_length(const std::uint32_t length) noexcept { _pcklen_max = length; }

		/*! Retrieve the link type for the packets that this pcap file contain. */
		[[nodiscard]]
		link_type_t link_type() const noexcept { return _network; }
		/*! Set the link type for the packets that this pcap file contain. */
		void link_type(const link_type_t type) noexcept { _network = type; }
	};
}

#endif /* LIBNOKOGIRI_PCAP_HEADER_HH */
