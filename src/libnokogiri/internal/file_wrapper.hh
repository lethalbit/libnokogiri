// SPDX-License-Identifier: LGPL-3.0-or-later
/* file_wrapper.hh - generic wrapper methods for dealing with gzfile_t and fd_t variants */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_FILE_WRAPPER_HH)
#define LIBNOKOGIRI_INTERNAL_FILE_WRAPPER_HH

#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fd.hh>
#include <libnokogiri/internal/zlib.hh>

namespace libnokogiri::internal {
	using capture_file_t = std::variant<libnokogiri::internal::fd_t, libnokogiri::internal::gzfile_t>;

	template<typename... Ts>
	struct overload_t : Ts... { using Ts::operator()...; };
	template<typename... Ts>
	overload_t(Ts...) -> overload_t<Ts...>;

	template<typename T>
	[[nodiscard]]
	bool write(const T& data, capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[&data](const libnokogiri::internal::fd_t& _file) { return _file.write<T>(data); },
				[&data](const libnokogiri::internal::gzfile_t& _file) { return _file.write<T>(data); }
			}, file);
	}

	template<typename T>
	[[nodiscard]]
	std::optional<T> read(capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[](const libnokogiri::internal::fd_t& _file) -> std::optional<T> {
					T local{};
					const auto res{_file.read(local)};
					return res ? std::optional<T>{local} : std::nullopt;
				},
				[](libnokogiri::internal::gzfile_t& _file) { return _file.read<T>(); }
			}, file);
	}

	template<typename T, std::size_t len>
	[[nodiscard]]
	bool write(const std::array<T, len>& data, capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[&data](const libnokogiri::internal::fd_t& _file) { return _file.write<len, T, len>(data); },
				[&data](const libnokogiri::internal::gzfile_t& _file) { return _file.write<T, len>(data); }
			}, file);
	}

	template<typename T, std::size_t len>
	[[nodiscard]]
	bool read(std::array<T, len>& data, capture_file_t& file) noexcept {
		return std::visit(libnokogiri::internal::overload_t {
				[&data](const libnokogiri::internal::fd_t& _file) { return _file.read<len, T, len>(data); },
				[&data](libnokogiri::internal::gzfile_t& _file) { return _file.read<T, len>(data); }
			}, file);
	}

	[[nodiscard]]
	LIBNOKOGIRI_API std::size_t seek(const std::size_t offset, const std::int32_t mode, capture_file_t& file) noexcept;
	[[nodiscard]]
	LIBNOKOGIRI_API bool eof(capture_file_t& file) noexcept;
}

#endif /* LIBNOKOGIRI_INTERNAL_FILE_WRAPPER_HH */
