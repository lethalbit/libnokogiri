// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/zlib.hh - libnokogiri zlib wrapper */
#if !defined(LIBNOKOGIRI_INTERNAL_ZLIB_HH)
#define LIBNOKOGIRI_INTERNAL_ZLIB_HH

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fs.hh>
#include <libnokogiri/internal/fd.hh>

extern "C" {
	#include <zlib.h>
	#include <zconf.h>
}

namespace libnokogiri::internal {

	struct gzfile_t final {
		fd_t _file;
		std::unique_ptr<gzFile_s, decltype(&gzclose)> _gz_file;
		std::int32_t _error{Z_OK};
	public:
		gzfile_t() noexcept :
			_file{-1}, _gz_file{nullptr, nullptr}
			{ /* NOP */ }

		gzfile_t(fd_t& file) noexcept :
			_file{std::move(file)}, _gz_file(gzdopen(_file, "r"), gzclose)
			{ /* NOP */ }

		gzfile_t(const fs::path& file, std::int32_t flags = O_RDONLY, mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) noexcept :
			_file{file, flags, mode}, _gz_file(gzdopen(_file, "r"), gzclose)
			{ /* NOP */ }

		gzfile_t(gzfile_t&& gz_file) noexcept : _file{}, _gz_file{nullptr, nullptr} { swap(gz_file); }
		void operator=(gzfile_t &&gz_file) noexcept { swap(gz_file); }

		gzfile_t(const gzfile_t&) = delete;
		gzfile_t& operator=(const gzfile_t&) = delete;

		[[nodiscard]]
		bool valid() const noexcept { return _file.valid() && _gz_file != nullptr; }

		template<typename T, std::size_t len>
		[[nodiscard]]
		bool read(std::array<T, len>& data) noexcept {
			return gzread(_gz_file.get(), data.data(), len * sizeof(T)) != -1;
		}

		template<typename T>
		[[nodiscard]]
		std::optional<T> read() noexcept {
			T local_storage{};
			const auto res = gzread(_gz_file.get(), &local_storage, sizeof(T));
			return (res != -1) ? std::optional<T>{local_storage} : std::nullopt;
		}

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<std::is_integral_v<T>, std::optional<T>>
		bswap_read() noexcept {
			T local_storage{};
			const auto res = gzread(_gz_file.get(), &local_storage, sizeof(T));

			if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
				local_storage = LIBNOKOGIRI_SWAP16(local_storage);
			} else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
				local_storage = LIBNOKOGIRI_SWAP32(local_storage);
			} else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
				local_storage = LIBNOKOGIRI_SWAP64(local_storage);
			}

			return (res != -1) ? std::optional<T>{local_storage} : std::nullopt;
		}

		template<typename T, std::size_t len>
		[[nodiscard]]
		bool write(const std::array<T, len>& data) noexcept {
			return gzwrite(_gz_file.get(), data.data(), len * sizeof(T)) != 0;
		}

		template<typename T>
		[[nodiscard]]
		bool write(const T& data) noexcept {
			return gzwrite(_gz_file.get(), &data, sizeof(T)) != 0;
		}

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<std::is_integral_v<T>, bool>
		bswap_write(const T& data) noexcept {
			T local_storage{};

			if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
				local_storage = LIBNOKOGIRI_SWAP16(data);
			} else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
				local_storage = LIBNOKOGIRI_SWAP32(data);
			} else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
				local_storage = LIBNOKOGIRI_SWAP64(data);
			}

			return write(local_storage);
		}

		[[nodiscard]]
		std::size_t seek(const std::size_t offset, const std::int32_t mode = SEEK_SET) noexcept {
			return static_cast<std::size_t>(gzseek(_gz_file.get(), offset, mode));
		}

		[[nodiscard]]
		std::size_t tell() const noexcept { return static_cast<std::size_t>(gztell(_gz_file.get())); }

		[[nodiscard]]
		bool eof() const noexcept { return gzeof(_gz_file.get()); }

		[[nodiscard]]
		bool flush(std::int32_t flush_mode) noexcept { return (_error = gzflush(_gz_file.get(), flush_mode)) == Z_OK; }

		[[nodiscard]]
		std::size_t length() noexcept { return _file.length(); }

		[[nodiscard]]
		const char* last_error_str() noexcept { return gzerror(_gz_file.get(), nullptr); }

		[[nodiscard]]
		std::int32_t last_error() noexcept { return _error; }

		std::int32_t decompress_to(fs::path& filename) {
			auto file = libnokogiri::internal::fd_t{filename, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH};
			std::int32_t decmpressed{};
			std::int32_t gzread_ret{};
			std::array<uint8_t, 8_KiB> tmp{};

			while ((gzread_ret = gzread(_gz_file.get(), tmp.data(), 8_KiB)) == 8_KiB) {
				const auto ret = file.write(tmp);
				if (!ret) {
					return -1;
				}
				decmpressed += gzread_ret;
				tmp.fill(0x00U);
			}

			if (gzread_ret == -1) {
				return -1;
			}

			for (std::size_t idx{}; idx < gzread_ret; ++idx) {
				const auto ret = file.write(tmp[idx]);
				if (!ret) {
					return -1;
				}
			}

			decmpressed += gzread_ret;

			return decmpressed;
		}

		void clear_error() noexcept { gzclearerr(_gz_file.get()); _error = 0; }

		void swap(gzfile_t &desc) noexcept {
			std::swap(_file, desc._file);
			std::swap(_gz_file, desc._gz_file);
			std::swap(_error, desc._error);
		}

	};

	inline void swap(gzfile_t &a, gzfile_t &b) noexcept { a.swap(b); }
}

#endif /* LIBNOKOGIRI_INTERNAL_ZLIB_HH */
