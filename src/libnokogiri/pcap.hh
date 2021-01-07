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
#include <libnokogiri/internal/iterator.hh>

#include <libnokogiri/pcap/header.hh>
#include <libnokogiri/pcap/packet.hh>

namespace libnokogiri::pcap {

	/*! \struct pcap_t
		\brief pcap file container

		This structure contains the machinery to read, write, and edit pcap files.

		The structure of a pcap file is a file header (pcap_header_t) followed
		by a collection of packet header and packet data pairs. This is all optionally
		gz compressed.

	*/
	struct LIBNOKOGIRI_CLS_API pcap_t final {
	public:
		using iterator_t = libnokogiri::internal::bi_iterator<
			std::optional<std::reference_wrapper<packet_t>>,
			packet_storage_t,
			std::vector<packet_storage_t>::iterator
		>;

	private:
		libnokogiri::internal::fd_t _file;
		capture_compression_t _compression;
		bool _readonly;
		bool _prefetch;
		file_header_t _header{};

		bool _valid{false};
		bool _needs_swapping{false};

		std::vector<packet_storage_t> _packets;

		bool read_header() noexcept;
		bool ingest_packets() noexcept;
	public:
		constexpr pcap_t() = delete;

		/*! \brief Construct a new pcap file container

			\param file The path to the pcap file
			\param compression The compression mode for the pcap file
			\param read_only Open the pcap file in read only
			\param prefetch Rather than initially building a packet index and then doing I/O to get each packet, ingest all packets at once, this trades memory usage for speed
		*/
		pcap_t(libnokogiri::internal::fs::path& file, capture_compression_t compression, bool read_only, bool prefetch = false) noexcept;

		pcap_t(const pcap_t&) = delete;
		pcap_t& operator=(const pcap_t&) = delete;

		pcap_t(pcap_t&& capture) noexcept :
			_file{},_compression{}, _readonly{true}
			{ swap(capture); }
		void operator=(pcap_t&& capture) noexcept { swap(capture); }

		[[nodiscard]]
		bool needs_swapping() const noexcept { return _needs_swapping; }

		[[nodiscard]]
		file_header_t& header() noexcept { return _header; }
		void header(file_header_t&& header) noexcept { _header = std::move(header); }

		[[nodiscard]]
		capture_compression_t compression_type() const noexcept { return _compression; }

		[[nodiscard]]
		bool valid() const noexcept { return _valid; }

		[[nodiscard]]
		std::size_t packet_count() const noexcept { return _packets.size(); }

		[[nodiscard]]
		bool save() const noexcept;

		void swap(pcap_t& desc) noexcept {
			std::swap(_file, desc._file);
			std::swap(_compression, desc._compression);
			std::swap(_readonly, desc._readonly);
			std::swap(_header, desc._header);
			std::swap(_valid, desc._valid);
		}


		void remove_packet(std::size_t index) noexcept {  }

		std::optional<std::reference_wrapper<packet_t>> get_packet(std::size_t idx) noexcept {
			if (idx <= _packets.size()) {
				return get_packet(std::ref(_packets[idx]));
			}
			return std::nullopt;
		}

		std::optional<std::reference_wrapper<packet_t>> get_packet(packet_storage_t& pkt_storage) noexcept;

		iterator_t begin() noexcept {
			return iterator_t([this](packet_storage_t& pkt_storage) -> std::optional<std::reference_wrapper<packet_t>> {
				return get_packet(pkt_storage);
			}, _packets.begin(), _packets.end(), true);
		}

		iterator_t end() noexcept {
			return iterator_t([this](packet_storage_t& pkt_storage) -> std::optional<std::reference_wrapper<packet_t>> {
				return get_packet(pkt_storage);
			}, _packets.begin(), _packets.end(), false);
		}
	};

	inline void swap(pcap_t& a, pcap_t& b) noexcept { a.swap(b); }
}

#endif /* LIBNOKOGIRI_PCAP_HH */
