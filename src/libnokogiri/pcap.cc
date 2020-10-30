// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap.cc - pcap file format interface for libnokogiri */


#include <cstdio>
#include <string>
#include <optional>

#include <libnokogiri/pcap.hh>

#include <libnokogiri/internal/zlib.hh>

namespace fs = libnokogiri::internal::fs;

namespace libnokogiri::pcap {

	pcap_t::pcap_t(libnokogiri::internal::fs::path& file, captrue_compression_t compression, bool read_only) noexcept :
		_file{}, _compression{compression}, _readonly{read_only} {
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
		} else {
			_file = std::move(cap);
		}



		if (!read_header()) {
			return;
		}

		_valid = true;
	}


	bool pcap_t::read_header() noexcept {
		// if (auto magic = libnokogiri::internal::read<std::uint32_t>(_file)) {
		// 	const auto pcap_magic{static_cast<pcap_variant_t>(*magic)};
		// 	switch (pcap_magic) {
		// 		case pcap_variant_t::Standard:
		// 		case pcap_variant_t::Modified:
		// 		case pcap_variant_t::IXIAHW:
		// 		case pcap_variant_t::IXIASW:
		// 		case pcap_variant_t::Nanosecond: {
		// 			/* Swapping is disabled by default, no need to do so */
		// 			_header.variant(pcap_magic);
		// 			break;
		// 		}
		// 		case pcap_variant_t::SwappedStantard:
		// 		case pcap_variant_t::SwappedModified:
		// 		case pcap_variant_t::SwappedIXIAHW:
		// 		case pcap_variant_t::SwappedIXIASW:
		// 		case pcap_variant_t::SwappedNanosecond: {
		// 			/* We need to swap all the things when reading */
		// 			_needs_swapping = true;
		// 			_header.variant(static_cast<pcap_variant_t>(LIBNOKOGIRI_SWAP32(*magic)));
		// 			break;
		// 		} default : {
		// 			/* Unknown file, bail */
		// 			return false;
		// 		}
		// 	}
		// } else {
		// 	return false;
		// }

		// const auto major = libnokogiri::internal::read<std::uint16_t>(_file);
		// const auto minor = libnokogiri::internal::read<std::uint16_t>(_file);
		// if(major && minor) {
		// 	_header.version(
		// 		(_needs_swapping) ?
		// 		version_t{LIBNOKOGIRI_SWAP16(*major), LIBNOKOGIRI_SWAP16(*minor)} :
		// 		version_t{*major, *minor}
		// 	);

		// } else {
		// 	return false;
		// }


		// if (auto tz_offset = libnokogiri::internal::read<std::int32_t>(_file)) {
		// 	_header.timezone_offset((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*tz_offset) : *tz_offset);
		// } else {
		// 	return false;
		// }

		// if (auto ts_figs = libnokogiri::internal::read<std::uint32_t>(_file)) {
		// 	_header.timestamp_accuracy((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*ts_figs) : *ts_figs);
		// } else {
		// 	return false;
		// }

		// if (auto pckt_len = libnokogiri::internal::read<std::uint32_t>(_file)) {
		// 	_header.max_packet_length((_needs_swapping) ? LIBNOKOGIRI_SWAP32(*pckt_len) : *pckt_len);
		// } else {
		// 	return false;
		// }

		// if (auto lnk_type = libnokogiri::internal::read<std::uint32_t>(_file)) {
		// 	_header.link_type(
		// 		(_needs_swapping) ?
		// 		static_cast<link_type_t>(LIBNOKOGIRI_SWAP32(*lnk_type)) :
		// 		static_cast<link_type_t>(*lnk_type)
		// 	);
		// } else {
		// 	return false;
		// }

		// return true;

		return false;
	}
}
