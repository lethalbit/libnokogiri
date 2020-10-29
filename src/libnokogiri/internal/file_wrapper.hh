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
		template<typename... Ts>
		struct overload_t : Ts... { using Ts::operator()...; };
		template<typename... Ts>
		overload_t(Ts...) -> overload_t<Ts...>;

		using capture_file_t = std::variant<libnokogiri::internal::fd_t, libnokogiri::internal::gzfile_t>;

		template<typename T>
		[[nodiscard]]
		LIBNOKOGIRI_API bool write(T& data, capture_file_t& file) noexcept;
		template<typename T>
		[[nodiscard]]
		LIBNOKOGIRI_API std::optional<T> read(capture_file_t& file) noexcept;

		template<typename T, std::size_t len>
		[[nodiscard]]
		LIBNOKOGIRI_API bool write(std::array<T, len>& data, capture_file_t& file) noexcept;
		template<typename T, std::size_t len>
		[[nodiscard]]
		LIBNOKOGIRI_API bool read(std::array<T, len>& data, capture_file_t& file) noexcept;

		[[nodiscard]]
		LIBNOKOGIRI_API std::size_t seek(capture_file_t& file, std::size_t offset, std::int32_t mode) noexcept;

		[[nodiscard]]
		LIBNOKOGIRI_API bool eof(capture_file_t& file) noexcept;
}

#endif /* LIBNOKOGIRI_INTERNAL_FILE_WRAPPER_HH */
