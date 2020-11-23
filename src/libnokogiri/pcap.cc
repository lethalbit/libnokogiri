// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap.cc - pcap file format interface for libnokogiri */


#include <cstdio>
#include <string>
#include <optional>

#include <libnokogiri/pcap.hh>

#include <libnokogiri/internal/zlib.hh>

#include <iostream>

namespace fs = libnokogiri::internal::fs;

namespace libnokogiri::pcap {

	pcap_t::pcap_t(libnokogiri::internal::fs::path& file, captrue_compression_t compression, bool read_only, bool prefetch) noexcept :
		_file{}, _compression{compression}, _readonly{read_only}, _prefetch{prefetch} {
		libnokogiri::internal::fd_t cap{file, (read_only) ? O_RDONLY : O_RDWR};
		if (_compression == captrue_compression_t::Autodetect) {
			_compression = libnokogiri::internal::detect_captrue_compression(cap);
		}

		if (_compression == captrue_compression_t::Compressed) {
			_file = std::move(libnokogiri::internal::fd_t::maketemp(O_RDWR,  S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, ".pcap"sv));
			libnokogiri::internal::gzfile_t gzcap{cap};
			if (gzcap.decompress_to(_file) == -1) {
				return;
			}

			if(!_file.head()) {
				return;
			}
		} else {
			_file = std::move(cap);
		}

		if (!read_header()) {
			return;
		}

		if (!ingest_packets()) {
			return;
		}

		_valid = true;
	}

	bool pcap_t::read_header() noexcept {
		if (auto magic = _file.read<std::uint32_t>()) {
			const auto pcap_magic{static_cast<pcap_variant_t>(*magic)};
			switch (pcap_magic) {
				case pcap_variant_t::Standard:
				case pcap_variant_t::Modified:
				case pcap_variant_t::IXIAHW:
				case pcap_variant_t::IXIASW:
				case pcap_variant_t::Nanosecond: {
					/* Swapping is disabled by default, no need to do so */
					_header.variant(pcap_magic);
					break;
				}
				case pcap_variant_t::SwappedStantard:
				case pcap_variant_t::SwappedModified:
				case pcap_variant_t::SwappedIXIAHW:
				case pcap_variant_t::SwappedIXIASW:
				case pcap_variant_t::SwappedNanosecond: {
					/* We need to swap all the things when reading */
					_needs_swapping = true;
					_header.variant(static_cast<pcap_variant_t>(LIBNOKOGIRI_SWAP32(*magic)));
					break;
				} default : {
					/* Unknown file, bail */
					return false;
				}
			}
		} else {
			return false;
		}

		const auto majorv = _file.read<std::uint16_t>();
		const auto minorv = _file.read<std::uint16_t>();
		if(majorv && minorv) {
			_header.version(
				(_needs_swapping) ?
				version_t{LIBNOKOGIRI_SWAP16(*majorv), LIBNOKOGIRI_SWAP16(*minorv)} :
				version_t{*majorv, *minorv}
			);
		} else {
			return false;
		}


		if (auto tz_offset = _file.read<std::int32_t>()) {
			_header.timezone_offset((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*tz_offset) : *tz_offset);
		} else {
			return false;
		}

		if (auto ts_figs = _file.read<std::uint32_t>()) {
			_header.timestamp_accuracy((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*ts_figs) : *ts_figs);
		} else {
			return false;
		}

		if (auto pckt_len = _file.read<std::uint32_t>()) {
			_header.max_packet_length((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*pckt_len) : *pckt_len);
		} else {
			return false;
		}

		if (auto lnk_type = _file.read<std::uint32_t>()) {
			_header.link_type(
				(_needs_swapping) ?
				static_cast<link_type_t>(LIBNOKOGIRI_SWAP32(*lnk_type)) :
				static_cast<link_type_t>(*lnk_type)
			);
		} else {
			return false;
		}

		return true;
	}

	std::size_t get_packet_body_offset(pcap_variant_t v) {
		switch (v) {
			case pcap_variant_t::Modified: {
				return 12U;
			} case pcap_variant_t::Standard:
			case pcap_variant_t::IXIAHW:
			case pcap_variant_t::IXIASW:
			case pcap_variant_t::Nanosecond:
			default: {
				return 4U;
			}
		};
	}

	/*
		The idea behind this is fairly simple.

		Because we are only interested in the packet offset and the size of
		the data on the first pass, we can skip most if not all of the other
		needed read calls.

		Seeing as the packet headers are all fixed sizes, and all share the
		common standard header we can seek to a known offset to extract the
		length of the packet data, then based on the packet header type, add
		that to our offset to get the proper offset for the packet data.

		So it works as follows:

			Assume the offset of _file is at the end of the file header.

			Get the packet header offset for the type of headers we expect.

			Until we reach the end of the file:

				Seek 8 bytes, skips the time-stamp members of the packet header

				Extract the next 4 bytes which is the size of the packet data

				Seek the remaining offset 4 for standard packets and 12 for modified

				Get the current seek position, set that as the offset.

				Seek to the current position + packet length

	*/
	bool pcap_t::ingest_packets() noexcept {
		const auto pkt_body_offset = get_packet_body_offset(_header.variant());
		const std::size_t pkt_len_offset = 8U;

		while (!_file.isEOF()) {
			const auto prev_pos = _file.tell();
			if(_file.seek(pkt_len_offset) != (pkt_len_offset + prev_pos))
				return false;

			/* Returns an optional */
			const auto size = _file.read<std::uint32_t>();
			if (!size) {
				return false;
			}

			_packets.emplace_back(packet_storage_t{*size, std::uintptr_t(prev_pos)});
			const auto pckt_size = _file.tell();
			const auto next_packet =  *size + pkt_body_offset;
			if(_file.seek(next_packet) != (next_packet + pckt_size))
				return false;
		}

		return true;
	}

	std::optional<std::reference_wrapper<packet_t>> pcap_t::get_packet(packet_storage_t& pkt_storage) noexcept {
		if (_file.seek(pkt_storage.offset(), SEEK_SET) != pkt_storage.offset()) {
			return std::nullopt;
		}

		packet_t::pkt_header_t header{};
		/* extract the header */
		switch (_header.variant()) {
			case pcap_variant_t::Modified: {
				if (const auto hdr = _file.read<packet_header_modified_t>()) {
					header.emplace<packet_header_modified_t>(*hdr);
				} else {
					return std::nullopt;
				}
				break;
			} case pcap_variant_t::Standard:
			case pcap_variant_t::IXIAHW:
			case pcap_variant_t::IXIASW:
			case pcap_variant_t::Nanosecond:
			default: {
				if (const auto hdr = _file.read<packet_header_t>()) {
					header.emplace<packet_header_t>(*hdr);
				} else {
					return std::nullopt;
				}
				break;
			}
		}

		packet_t packet{
			std::visit([](auto& header) -> std::size_t {
				using T = std::decay_t<decltype(header)>;
				if constexpr (std::is_same_v<T, packet_header_modified_t>) {
					return header.base_header().captured_len();
				} else if constexpr (std::is_same_v<T, packet_header_t>) {
					return header.captured_len();
				} else {
					return 0U;
				}
			}, header),
			header
		};

		/* ingest the body */
		const auto ingested = _file.read(packet.address(0), packet.length());

		if (ingested) {
			pkt_storage.set_packet(std::move(packet));

			return std::make_optional(std::ref(pkt_storage.get_packet()));
		}

		return std::nullopt;
	}
}
