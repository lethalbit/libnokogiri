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
	private:
		libnokogiri::internal::fd_t _file;
		captrue_compression_t _compression;
		bool _readonly;
		file_header_t _header{};

		bool _valid{false};
		bool _needs_swapping{false};

		std::vector<packet_storage_t> _packets;

		bool read_header() noexcept;
		bool ingest_packets() noexcept;
	public:
		constexpr pcap_t() = delete;

		pcap_t(libnokogiri::internal::fs::path& file, captrue_compression_t compression, bool read_only) noexcept;

		pcap_t(const pcap_t&) = delete;
		pcap_t& operator=(const pcap_t&) = delete;

		pcap_t(pcap_t&& capture) noexcept :
			_file{},_compression{}, _readonly{true}
			{ swap(capture); }
		void operator=(pcap_t&& capture) noexcept { swap(capture); }

		[[nodiscard]]
		bool needs_swapping() const noexcept { return _needs_swapping; }

		[[nodiscard]]
		file_header_t header() const noexcept { return _header; }
		void header(file_header_t header) noexcept { _header = header; }

		[[nodiscard]]
		captrue_compression_t compression_type() const noexcept { return _compression; }

		[[nodiscard]]
		bool valid() const noexcept { return _valid; }

		[[nodiscard]]
		std::size_t packet_count() const noexcept { return _packets.size(); }

		auto begin() const noexcept { return _packets.begin(); }
		auto end() const noexcept { return _packets.end(); }

		[[nodiscard]]
		bool save() const noexcept;

		void swap(pcap_t& desc) noexcept {
			std::swap(_file, desc._file);
			std::swap(_compression, desc._compression);
			std::swap(_readonly, desc._readonly);
			std::swap(_header, desc._header);
			std::swap(_valid, desc._valid);
		}
	};

	inline void swap(pcap_t& a, pcap_t& b) noexcept { a.swap(b); }
}

#endif /* LIBNOKOGIRI_PCAP_HH */
