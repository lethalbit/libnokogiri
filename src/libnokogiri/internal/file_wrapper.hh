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
		LIBNOKOGIRI_API bool write(const T& data, capture_file_t& file) noexcept;
		template<typename T>
		[[nodiscard]]
		LIBNOKOGIRI_API std::optional<T> read(capture_file_t& file) noexcept;

		template<typename T, std::size_t len>
		[[nodiscard]]
		LIBNOKOGIRI_API bool write(const std::array<T, len>& data, capture_file_t& file) noexcept;
		template<typename T, std::size_t len>
		[[nodiscard]]
		LIBNOKOGIRI_API bool read(std::array<T, len>& data, capture_file_t& file) noexcept;

		[[nodiscard]]
		LIBNOKOGIRI_API std::size_t seek(const std::size_t offset, const std::int32_t mode, capture_file_t& file) noexcept;

		[[nodiscard]]
		LIBNOKOGIRI_API bool eof(capture_file_t& file) noexcept;
}

#endif /* LIBNOKOGIRI_INTERNAL_FILE_WRAPPER_HH */
