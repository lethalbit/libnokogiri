// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap.hh - pcap file format interface for libnokogiri */
#if !defined(LIBNOKOGIRI_PCAP_HH)
#define LIBNOKOGIRI_PCAP_HH

#include <cstdint>
#include <memory>
#include <optional>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fs.hh>

#if defined(LIBNOKOGIRI_COMPRESSED_PCAP_SUPPORTED)
#	include <libnokogiri/internal/zlib.hh>
#endif

namespace libnokogiri::pcap {

	/*! \enum libnokogiri::pcap::pcap_variant_t
		\brief Types of possible pcap formats
	 */
	enum struct pcap_variant_t : std::uint32_t {
		Standard   = 0xA1B2C3D4U, /*! "Standard" pcap file magic  */
		Modified   = 0xA1B2CD34U, /*! Magic for the modified pcap format introduced by Alexey Kuznetsov's patches to some versions of libpcap */
		IXIAHW     = 0x1C0001ACU, /*! Magic for IXIA's lcap format */
		IXIASW     = 0x1C0001ABU, /*! Magic for IXIA's lcap format */
		Nanosecond = 0x4D3CB2A1U, /*! Magic for the modified pcap format introduced by Ulf Lamping's patches. It's identical to the standard but the timestamps are in nanoseconds. */
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

		file_header_t(std::nullptr_t) noexcept { /* NOP */ }

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

	/*! \struct libnokogiri::pcap::packet_header_t
		\brief The packet header for normal unmodified pcap file packets

		This structure represents the header that is prior to each block of
		packet data.

		The packet headers have the following structure

		```
		 0               1               2               3
		 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                            Seconds                            |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                         Microseconds                          |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                        Captured Length                        |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                      Full Packet Length                       |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```

		 - `Seconds` - An unsigned 32-bit unix timestamp when the packet was captured. This can be adjusted to GMT with the timezone offset in the file header.
		 - `Microseconds` - An unsigned 32-bit value representing either the microseconds when the packet was captured if standard, or the nanoseconds when captured if using the modified format.
		 - `Captured Length` - An unsigned 32-bit value representing the number of octets captured that have been saved to the file.
		 - `Full Packet Length` - An unsigned 32-bit value representing the total number of octets in the packet.

		Due to resource limitations, it is possible for a packet capture to not have
		the complete contents of the packet, therefore the packet headers keep track
		of how big the packet was, as well as how much of that was actually written
		to the capture.
	*/
	struct packet_header_t final {
	private:
		std::uint32_t _timestamp;
		std::uint32_t _usecs;
		std::uint32_t _have;
		std::uint32_t _was;
	public:
		constexpr packet_header_t() noexcept :
			_timestamp{0U}, _usecs{0U}, _have{0U}, _was{0U}
			{ /* NOP */ }

		constexpr packet_header_t(std::uint32_t timestamp, std::uint32_t useconds,
				std::uint32_t pkt_len_have, std::uint32_t pkt_len_actual) noexcept :
			_timestamp{timestamp}, _usecs{useconds}, _have{pkt_len_have}, _was{pkt_len_actual}
			{ /* NOP */ }

		packet_header_t(std::nullptr_t) noexcept { /* NOP */ }


		/*! Retrieve the packets timestamp */
		[[nodiscard]]
		std::uint32_t timestamp() const noexcept { return _timestamp; }
		/*! Set the packet timestamp */
		void timestamp(const std::uint32_t timestamp) noexcept { _timestamp = timestamp; }

		/*! Retrieve the micro/nano second value for the packet */
		[[nodiscard]]
		std::uint32_t useconds() const noexcept { return _usecs; }
		/*! Set the micro/nano second value for the packet */
		void useconds(const std::uint32_t useconds) noexcept { _usecs = useconds; }

		/*! Retrieve the captured length of the packet */
		[[nodiscard]]
		std::uint32_t captured_len() const noexcept { return _have; }
		/*! Set the captured length of the packet */
		void captured_len(const std::uint32_t captured_len) noexcept { _have = captured_len; }

		/*! Retrieve the actual length of the packet */
		[[nodiscard]]
		std::uint32_t actual_len() const noexcept { return _was; }
		/*! Set the actual length of the packet */
		void actual_len(const std::uint32_t actual_len) noexcept { _was = actual_len; }

		/*! Checks if the packet represented by this header is a full packet */
		[[nodiscard]]
		bool full_packet() const noexcept { return _was == _have; }
	};

	/*! \struct libnokogiri::pcap::packet_header_modified_t
		\brief The packet header for the patched version of libpcap

		This header is an extension of the normal packet header, it adds four fields
		to the end of the traditional header.

		The packet headers have the following structure

		```
		 0               1               2               3
		 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                                                               |
		+                                                               +
		|                                                               |
		+                         Packet Header                         +
		|                                                               |
		+                                                               +
		|                                                               |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|                        Interface Index                        |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		|           Protocol            |      Type     |    Padding    |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```
		 - `Packet Header` - The standard pcap packet header represented by the libnokogiri::pcap::packet_header_t structure.
		 - `Interface Index` - An unsigned 32-bit integer representing the index of the interface on which the capturing machine this packet came from.
		 - `Protocol` - An unsigned 16-bit value representing the Ethernet packet type. Not to be confused with `Type`.
		 - `Type` - An unsigned 8-bit value representing if the packet was broadcast/multicast/etc.
		 - `Padding` - 8-bits of padding to pad the structure to a 32-bit boundary.
	*/
	struct packet_header_modified_t final {
	private:
		packet_header_t _base_header;
		std::uint32_t _if_index;
		std::uint16_t _proto;
		std::uint8_t _type;
		std::uint8_t _padding;
	public:
		constexpr packet_header_modified_t() noexcept :
			_base_header{}, _if_index{0U}, _proto{0U}, _type{0U}, _padding{0U}
			{ /* NOP */ }

		constexpr packet_header_modified_t(packet_header_t base_header, std::uint32_t if_index,
				std::uint16_t protocol, std::uint8_t type) noexcept :
			_base_header{base_header}, _if_index{if_index}, _proto{protocol}, _type{type},
			_padding{0U} { /* NOP */ }

		packet_header_modified_t(std::nullptr_t) noexcept { /* NOP */ }


		/*! Retrieve the base packet header */
		[[nodiscard]]
		packet_header_t base_header() const noexcept { return _base_header; }
		/*! Set the base packet header */
		void base_header(packet_header_t base_header) noexcept { _base_header = base_header; }

		/*! Retrieve the interface index for this packet */
		[[nodiscard]]
		std::uint32_t interface_index() const noexcept { return _if_index; }
		/*! Set the interface index for this packet */
		void interface_index(std::uint32_t interface_index) noexcept { _if_index = interface_index; }

		/*! Retrieve protocol type for this packet */
		[[nodiscard]]
		std::uint16_t protocol() const noexcept { return _proto; }
		/*! Set protocol type for this packet */
		void protocol(std::uint16_t protocol) noexcept { _proto = protocol; }

		/*! Retrieve the type of this packet  */
		[[nodiscard]]
		std::uint8_t type() const noexcept { return _type; }
		/*! Set the type of this packet */
		void type(std::uint8_t type) noexcept { _type = type; }

	};


	/*! \struct libnokogiri::pcap::packet_t
		\brief pcap packet template

		This template provides the framework for reading and writing packets.

		They can have custom headers, but the two standardized packets are already
		specified as libnokogiri::pcap::generic_packet_t and libnokogiri::pcap::modified_packet_t .

		All this template does is change out the packet header type, the rest of the logic is identical.
	*/
	template<typename T, typename U>
	struct packet_t final {
	public:
		using data_t = std::optional<std::unique_ptr<U>>;
	private:
		T _packet_header;
		std::uintptr_t _offset;
		data_t _data;
	public:
		constexpr packet_t() noexcept :
			_packet_header{}, _offset{0U}, _data{std::nullopt}
			{ /* NOP */ }

		constexpr packet_t(T header, std::uintptr_t offset, data_t data) noexcept :
			_packet_header{header}, _offset{offset}, _data{data}
			{ /* NOP */ }

		/*! Retrieve the packet header */
		[[nodiscard]]
		T header() const noexcept { return _packet_header; }
		/*! Set the packet header */
		void header(T header) noexcept { _packet_header = header; }

		/*! Retrieve the offset in the file the packet is located at */
		[[nodiscard]]
		std::uintptr_t packet_offset() const noexcept { return _offset; }
		/*! Set the offset in the file the packet is located at */
		void packet_offset(std::uintptr_t packet_offset) noexcept { _offset = packet_offset; }

		/*! Retrieve the packet data */
		[[nodiscard]]
		data_t packet_data() const noexcept { return _data; }
		/*! Set the packet data */
		void packet_data(data_t packet_data) noexcept { _data = packet_data; }
	};

	/*! Type alias for generic conforming pcap packets with the standard header */
	template<typename T>
	using generic_packet_t = packet_t<packet_header_t, T>;
	/*! Type alias for packets from the modified pcap files */
	template<typename T>
	using modified_packet_t = packet_t<packet_header_modified_t, T>;

	/*! \struct pcap_t
		\brief pcap file container

		This structure contains the machinery to read, write, and edit pcap files.

		The structure of a pcap file is a file header (pcap_header_t) followed
		by a collection of packet header and packet data pairs. This is all optionally
		gz compressed.

	*/
	struct pcap_t final {
	public:

	};
}

#endif /* LIBNOKOGIRI_PCAP_HH */
