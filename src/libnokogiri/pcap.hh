// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap.hh - pcap file format interface for libnokogiri */
#if !defined(LIBNOKOGIRI_PCAP_HH)
#define LIBNOKOGIRI_PCAP_HH

#include <cstdint>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fs.hh>

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

		This structure represents the header of a pcap file.

		It contains a magic number which helps identify both the type of pcap file as well as which endian it is;
		The version of the file; The timezone offset relative to GMT used by the timestamps; The accuracy of the timestamps;
		The maximum length of the captured packets in octets; And the type of Link the capture corresponds to.

		The structure of this block in the file is as follows:

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
		|                        Data Link Type                         |
		+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		```

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

		[[nodiscard]]
		pcap_variant_t variant() const noexcept { return _variant; }
		void variant(const pcap_variant_t variant) noexcept { _variant = variant; }

		[[nodiscard]]
		version_t version() const noexcept { return _version; }
		void version(const version_t version) noexcept { _version = version; }

		[[nodiscard]]
		std::int32_t timezone_offset() const noexcept { return _tz_offset; }
		void timezone_offset(const std::int32_t tz_offset) noexcept { _tz_offset = tz_offset; }

		[[nodiscard]]
		std::uint32_t timestamp_accuracy() const noexcept { return _timestamp_figs; }
		void timestamp_accuracy(const std::uint32_t ts_accuracy) noexcept { _timestamp_figs = ts_accuracy; }

		[[nodiscard]]
		std::uint32_t max_packet_length() const noexcept { return _pcklen_max; }
		void max_packet_length(const std::uint32_t length) noexcept { _pcklen_max = length; }

		[[nodiscard]]
		link_type_t link_type() const noexcept { return _network; }
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

		Due to resource limitations, it is possible for a packet capture to not have
		the complete contents of the packet, therefore the packet headers keep track
		of how big the packet was, as well as how much of that was actually written
		to the capture.
	*/
	struct packet_header_t final {
	private:
		std::uint32_t _secs;
		std::uint32_t _usecs;
		std::uint32_t _have;
		std::uint32_t _was;
	public:
		constexpr packet_header_t() noexcept :
			_secs{0U}, _usecs{0U}, _have{0U}, _was{0U}
			{ /* NOP */ }

		constexpr packet_header_t(std::uint32_t seconds, std::uint32_t useconds,
				std::uint32_t pkt_len_have, std::uint32_t pkt_len_actual) noexcept :
			_secs{seconds}, _usecs{useconds}, _have{pkt_len_have}, _was{pkt_len_actual}
			{ /* NOP */ }

		packet_header_t(std::nullptr_t) noexcept { /* NOP */ }


		[[nodiscard]]
		std::uint32_t seconds() const noexcept { return _secs; }
		void seconds(const std::uint32_t seconds) noexcept { _secs = seconds; }

		[[nodiscard]]
		std::uint32_t useconds() const noexcept { return _usecs; }
		void useconds(const std::uint32_t useconds) noexcept { _usecs = useconds; }

		[[nodiscard]]
		std::uint32_t captured_len() const noexcept { return _have; }
		void captured_len(const std::uint32_t captured_len) noexcept { _have = captured_len; }

		[[nodiscard]]
		std::uint32_t actual_len() const noexcept { return _was; }
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


		[[nodiscard]]
		packet_header_t base_header() const noexcept { return _base_header; }
		void base_header(packet_header_t base_header) noexcept { _base_header = base_header; }

		[[nodiscard]]
		std::uint32_t interface_index() const noexcept { return _if_index; }
		void interface_index(std::uint32_t interface_index) noexcept { _if_index = interface_index; }

		[[nodiscard]]
		std::uint16_t protocol() const noexcept { return _proto; }
		void protocol(std::uint16_t protocol) noexcept { _proto = protocol; }

		[[nodiscard]]
		std::uint8_t type() const noexcept { return _type; }
		void type(std::uint8_t type) noexcept { _type = type; }

	};


	/*! \struct libnokogiri::pcap::packet_t
		\brief pcap packet template

		This template provides the framework for reading and writing packets.

		They can have custom headers, but the two standardized packets are already
		specified as libnokogiri::pcap::generic_packet_t and libnokogiri::pcap::modified_packet_t .

		All this template does is change out the packet header type, the rest of the logic is identical.
	*/
	template<typename T>
	struct packet_t final {
	private:
		T _packet_header;
		std::uintptr_t offset;
	public:

	};

	/*! Type alias for generic conforming pcap packets with the standard header */
	using generic_packet_t = packet_t<packet_header_t>;
	/*! Type alias for packets from the modified pcap files */
	using modified_packet_t = packet_t<packet_header_modified_t>;

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
