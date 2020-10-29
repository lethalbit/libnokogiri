// SPDX-License-Identifier: LGPL-3.0-or-later
/* file_wrapper.cc - generic wrapper methods for dealing with gzfile_t and fd_t variants */

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <optional>
#include <variant>

#include <libnokogiri/internal/file_wrapper.hh>

namespace libnokogiri::internal {

	template<typename T>
	[[nodiscard]]
	bool write(T& data, capture_file_t& file) noexcept {
		return std::visit(
			file,
			libnokogiri::internal::overload_t {
				[data](const libnokogiri::internal::fd_t& _file) -> std::optional<T> {
					T local{};
					const auto res{_file.read(local)};
					return res ? local : std::nullopt;
				},
				[data](const libnokogiri::internal::gzfile_t& _file) { return _file.read<T>(); }
			});
	}

	template<typename T>
	[[nodiscard]]
	std::optional<T> read(capture_file_t& file) noexcept {
		return std::visit(
			file,
			libnokogiri::internal::overload_t {
				[](const libnokogiri::internal::fd_t& _file) -> std::optional<T> {
					T local{};
					const auto res{_file.read(local)};
					return res ? local : std::nullopt;
				},
				[](const libnokogiri::internal::gzfile_t& _file) { return _file.read<T>(); }
			});
	}

	template<typename T, std::size_t len>
	[[nodiscard]]
	bool write(const std::array<T, len>& data, capture_file_t& file) noexcept {
		return std::visit(
			file,
			libnokogiri::internal::overload_t {
				[data](const libnokogiri::internal::fd_t& _file) { return _file.write<len, T, len>(data); },
				[data](const libnokogiri::internal::gzfile_t& _file) { return _file.write<T, len>(data); }
			});
	}

	template<typename T, std::size_t len>
	[[nodiscard]]
	bool read(std::array<T, len>& data, capture_file_t& file) noexcept {
		return std::visit(
			file,
			libnokogiri::internal::overload_t {
				[data](const libnokogiri::internal::fd_t& _file) { return _file.read<len, T, len>(data); },
				[data](const libnokogiri::internal::gzfile_t& _file) { return _file.read<T, len>(data); }
			});
	}

	// [[nodiscard]]
	// std::size_t seek(capture_file_t& file, std::size_t offset, std::int32_t mode) noexcept {
	// 	return std::visit(
	// 		file,
	// 		libnokogiri::internal::overload_t {
	// 			[offset, mode](const libnokogiri::internal::fd_t& _file) -> std::size_t {
	// 				return static_cast<std::size_t>(_file.seek(offset, mode));
	// 			},
	// 			[offset, mode](const libnokogiri::internal::gzfile_t& _file) -> std::size_t {
	// 				return _file.seek(offset, mode);
	// 			}
	// 		});
	// }

	// [[nodiscard]]
	// bool eof(capture_file_t& file) noexcept {
	// 	return std::visit(
	// 		file,
	// 		libnokogiri::internal::overload_t {
	// 			[](const libnokogiri::internal::fd_t& _file) { return _file.isEOF(); },
	// 			[](const libnokogiri::internal::gzfile_t& _file) { return _file.eof(); }
	// 		});
	// }
}
