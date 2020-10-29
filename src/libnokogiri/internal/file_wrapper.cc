// SPDX-License-Identifier: LGPL-3.0-or-later
/* file_wrapper.cc - generic wrapper methods for dealing with gzfile_t and fd_t variants */
#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/file_wrapper.hh>

namespace libnokogiri::internal {
	[[nodiscard]]
	std::size_t seek(const std::size_t offset, const std::int32_t mode, capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[offset, mode](const libnokogiri::internal::fd_t& _file) -> std::size_t {
					return static_cast<std::size_t>(_file.seek(offset, mode));
				},
				[offset, mode](libnokogiri::internal::gzfile_t& _file) -> std::size_t {
					return _file.seek(offset, mode);
				}
			}, file);
	}

	[[nodiscard]]
	bool eof(capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[](const libnokogiri::internal::fd_t& _file) -> bool { return _file.isEOF(); },
				[](const libnokogiri::internal::gzfile_t& _file) -> bool { return _file.eof(); }
			}, file);
	}
}
