// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap/packet.hh - libnokogiri pcap packets */
#if !defined(LIBNOKOGIRI_PCAP_PACKET_HH)
#define LIBNOKOGIRI_PCAP_PACKET_HH

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>
#include <array>
#include <string_view>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>

namespace libnokogiri::pcap {
	using libnokogiri::internal::enum_pair_t;
	/*! \enum libnokogiri::pcap::packet_type_t
		\brief Packet types

		This enum is sued in the libnokogiri::pcap::packet_storage_t container to allow
		you do differentiate between the standard packet types or the modified type.
	*/
	enum struct packet_type_t : uint8_t {
		Standard = 0x00, /*!< Standard packet type */
		Modified = 0x01, /*!< Modified packet type with extended header */
	};

	const std::array<const enum_pair_t<packet_type_t>, 2> packet_type_s{{
		{ packet_type_t::Standard, "Standard"sv },
		{ packet_type_t::Modified, "Modified"sv }
	}};

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

		packet_header_t(const packet_header_t&) = delete;
		packet_header_t& operator=(const packet_header_t&) = delete;

		packet_header_t(packet_header_t&&) = default;
		packet_header_t& operator=(packet_header_t&&) = default;

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

		constexpr packet_header_modified_t(packet_header_t&& base_header, std::uint32_t if_index,
				std::uint16_t protocol, std::uint8_t type) noexcept :
			_base_header{std::move(base_header)}, _if_index{if_index}, _proto{protocol}, _type{type},
			_padding{0U} { /* NOP */ }

		packet_header_modified_t(std::nullptr_t) noexcept { /* NOP */ }

		packet_header_modified_t(const packet_header_modified_t&) = delete;
		packet_header_modified_t& operator=(const packet_header_modified_t&) = delete;

		packet_header_modified_t(packet_header_modified_t&&) = default;
		packet_header_modified_t& operator=(packet_header_modified_t&&) = default;

		/*! Retrieve the base packet header */
		[[nodiscard]]
		const packet_header_t& base_header() const noexcept { return _base_header; }
		/*! Set the base packet header */
		void base_header(packet_header_t&& base_header) noexcept { _base_header = std::move(base_header); }

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


	// /*! \struct libnokogiri::pcap::packet_t
	// 	\brief pcap packet template

	// 	This template provides the framework for reading and writing packets.

	// 	They can have custom headers, but the two standardized packets are already
	// 	specified as libnokogiri::pcap::generic_packet_t and libnokogiri::pcap::modified_packet_t .

	// 	All this template does is change out the packet header type, the rest of the logic is identical.
	// */
	// template<typename T, typename U>
	// struct packet_t final {
	// public:
	// 	using data_t = std::optional<std::unique_ptr<U>>;
	// private:
	// 	T _packet_header;
	// 	std::uintptr_t _offset;
	// 	data_t _data;
	// public:
	// 	constexpr packet_t() noexcept :
	// 		_packet_header{}, _offset{0U}, _data{std::nullopt}
	// 		{ /* NOP */ }

	// 	constexpr packet_t(T header, std::uintptr_t offset, data_t data) noexcept :
	// 		_packet_header{header}, _offset{offset}, _data{data}
	// 		{ /* NOP */ }

	// 	/*! Retrieve the packet header */
	// 	[[nodiscard]]
	// 	T header() const noexcept { return _packet_header; }
	// 	/*! Set the packet header */
	// 	void header(T header) noexcept { _packet_header = header; }

	// 	/*! Retrieve the offset in the file the packet is located at */
	// 	[[nodiscard]]
	// 	std::uintptr_t packet_offset() const noexcept { return _offset; }
	// 	/*! Set the offset in the file the packet is located at */
	// 	void packet_offset(std::uintptr_t packet_offset) noexcept { _offset = packet_offset; }

	// 	/*! Retrieve the packet data */
	// 	[[nodiscard]]
	// 	data_t packet_data() const noexcept { return _data; }
	// 	/*! Set the packet data */
	// 	void packet_data(data_t packet_data) noexcept { _data = packet_data; }
	// };

	// /*! Type alias for generic conforming pcap packets with the standard header */
	// template<typename T>
	// using generic_packet_t = packet_t<packet_header_t, T>;
	// /*! Type alias for packets from the modified pcap files */
	// template<typename T>
	// using modified_packet_t = packet_t<packet_header_modified_t, T>;


	struct packet_t {
	public:
		using pkt_header_t = std::variant<
			std::monostate,
			packet_header_t,
			packet_header_modified_t
		>;
	private:
		std::vector<std::uint8_t> _raw_data;
		pkt_header_t _packet_header;

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<
			std::is_pod_v<T> &&
			!libnokogiri::internal::has_nullable_ctor_v<T> &&
			!std::is_same_v<T, void*>,
		T*>
		index(const std::size_t offset) {
			if (offset < _raw_data.size()) {
				return new (reinterpret_cast<std::uint8_t *>(_raw_data.data()) + (offset * sizeof(T))) T{};
			}
			return nullptr;
		}

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<
			libnokogiri::internal::has_nullable_ctor_v<T> &&
			!std::is_same_v<T, void*>,
		T*>
		index(const std::size_t offset) {
			if (offset < _raw_data.size()) {
				return new (reinterpret_cast<std::uint8_t *>(_raw_data.data()) + (offset * sizeof(T))) T{nullptr};
			}
			return nullptr;
		}

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<std::is_same_v<T, void*>, void*>
		index(const std::size_t offset) {
			if (offset < _raw_data.size()) {
				return reinterpret_cast<std::uint8_t *>(_raw_data.data()) + offset;
			}
			return nullptr;
		}

	public:

		packet_t(std::size_t length, pkt_header_t header = {}) noexcept :
			_raw_data{std::vector<std::uint8_t>(length)},
			_packet_header{std::move(header)} { /* NOP */ }


		packet_t(const packet_t&) = delete;
		packet_t& operator=(const packet_t&) = delete;

		packet_t(packet_t&&) = default;
		packet_t& operator=(packet_t&&) = default;

		[[nodiscard]]
		std::size_t length() const noexcept { return _raw_data.size(); }

		[[nodiscard]]
		pkt_header_t& header() noexcept { return _packet_header; }

		[[nodiscard]]
		bool is_complete() const noexcept {
			return std::visit([](auto& header) -> bool {
				using T = std::decay_t<decltype(header)>;
				if constexpr (std::is_same_v<T, packet_header_modified_t>) {
					return header.base_header().full_packet();
				} else if constexpr (std::is_same_v<T, packet_header_t>) {
					return header.full_packet();
				} else {
					return false;
				}
			}, _packet_header);
		}

		template<typename T>
		[[nodiscard]]
		T& as() { *index<T>(); }

		template<typename T>
		[[nodiscard]]
		T *operator [](const off_t idx) { return index<T>(idx); }

		[[nodiscard]]
		auto begin() noexcept { return _raw_data.begin(); }
		[[nodiscard]]
		auto end() noexcept { return _raw_data.end(); }

		template<typename T>
		[[nodiscard]]
		const T *operator [](const off_t idx) const { return index<const T>(idx); }

		template<typename T>
		[[nodiscard]]
		T *at(const off_t idx) { return index<T>(idx); }

		template<typename T>
		[[nodiscard]]
		const T *at(const off_t idx) const { return index<const T>(idx); }

		void *address(const off_t offset) noexcept { return index<void *>(offset); }
	};



	/*! \struct libnokogiri::pcap::packet_storage_t
		\brief Storage container for pcap packet data

		This structure stores a small amount of information pertaining to a packet
		within the pcap file.

		It holds the packet type, the size of the packet (including the header), the offset
		into the pcap file, and the cached packet itself if it's been read.
	*/
	struct packet_storage_t final {
	private:
		std::uint32_t _len;
		std::uintptr_t _offset;
		packet_t _packet_cache;
	public:
		packet_storage_t() noexcept :
			_len{0U}, _offset{0U}, _packet_cache{0}
			{ /* NOP */ }

		packet_storage_t(std::uint32_t len, std::uintptr_t offset) noexcept :
			_len{len}, _offset{offset}, _packet_cache{0}
			{ /* NOP */ }

		packet_storage_t(std::uint32_t len, std::uintptr_t offset, packet_t&& packet) noexcept :
			_len{len}, _offset{offset}, _packet_cache{std::move(packet)}
			{ /* NOP */ }

		packet_storage_t(const packet_storage_t&) = delete;
		packet_storage_t& operator=(const packet_storage_t&) = delete;

		packet_storage_t(packet_storage_t&&) = default;
		packet_storage_t& operator=(packet_storage_t&&) = default;

		[[nodiscard]]
		std::uint32_t length() const noexcept { return _len; }

		[[nodiscard]]
		std::uintptr_t offset() const noexcept { return _offset; }

		/*! Gets the packet that is represented by this storage object */
		[[nodiscard]]
		packet_t& get_packet() noexcept { return _packet_cache; }

		void set_packet(packet_t&& pkt) noexcept { _packet_cache = std::move(pkt); }
	};
}

#endif /* LIBNOKOGIRI_PCAP_PACKET_HH */
