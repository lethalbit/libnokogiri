// SPDX-License-Identifier: LGPL-3.0-or-later
/* common.cc - Common templates and type definitions */

#include <cstdint>
#include <algorithm>

#include <libnokogiri/common.hh>


namespace libnokogiri::internal {
	[[nodiscard]]
	captrue_compression_t detect_captrue_compression(fd_t& file) {
		constexpr static std::array<uint8_t, 2> gzip_header{0x1FU, 0x8BU};
		std::array<uint8_t, 2> read_bytes{};
		const auto res{file.read<2>(read_bytes)};
		file.head();

		if (!res) {
			return captrue_compression_t::Unknown;
		}

		const auto is_gzip = std::equal(
			gzip_header.begin(), gzip_header.end(),
			read_bytes.begin(), read_bytes.end()
		);

		return is_gzip ? captrue_compression_t::Compressed : captrue_compression_t::Uncompressed;
	}
}
