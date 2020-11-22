// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/fd.hh - Cross platform fd wrapper */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_FD_HH)
#define LIBNOKOGIRI_INTERNAL_FD_HH

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fs.hh>

#include <cstdint>
#include <cstddef>
#ifndef _WINDOWS
#	include <unistd.h>
#else
#	include <io.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <type_traits>
#include <fcntl.h>
#include <utility>
#include <memory>
#include <array>
#include <string>
#include <string_view>
#include <random>
#include <optional>
#include <algorithm>

#if defined(_MSC_VER) && !defined(_WINDOWS)
#define _WINDOWS 1
#endif

using namespace std::literals::string_view_literals;

namespace libnokogiri::internal {
#ifndef _WINDOWS
	using mode_t = ::mode_t;
	using ssize_t = ::ssize_t;
	using off_t = ::off_t;

	using stat_t = struct stat;
	using ::fstat;

	inline ssize_t fdread(const int32_t fd, void *const bufferPtr, const size_t bufferLen) noexcept
		{ return read(fd, bufferPtr, bufferLen); }
	inline ssize_t fdwrite(const int32_t fd, const void *const bufferPtr, const size_t bufferLen) noexcept
		{ return write(fd, bufferPtr, bufferLen); }
	inline off_t fdseek(const int32_t fd, const off_t offset, const int32_t whence) noexcept
		{ return lseek(fd, offset, whence); }
	inline off_t fdtell(const int32_t fd) noexcept
		{ return lseek(fd, 0, SEEK_CUR); }
	inline int32_t fdtruncate(const int32_t fd, const off_t size) noexcept
		{ return ftruncate(fd, size); }

#else
#	define O_NOCTTY _O_BINARY
	using mode_t = int32_t;
	using ssize_t = typename std::make_signed<std::size_t>::type;
	using off_t = int64_t;

	using stat_t = struct ::_stat64;

	inline ssize_t fdread(const int32_t fd, void *const bufferPtr, const size_t bufferLen) noexcept
		{ return read(fd, bufferPtr, uint32_t(bufferLen)); }
	inline ssize_t fdwrite(const int32_t fd, const void *const bufferPtr, const size_t bufferLen) noexcept
		{ return write(fd, bufferPtr, uint32_t(bufferLen)); }
	inline int fstat(int32_t fd, stat_t *stat) noexcept { return _fstat64(fd, stat); }
	inline off_t fdseek(const int32_t fd, const off_t offset, const int32_t whence) noexcept
		{ return _lseeki64(fd, offset, whence); }
	inline off_t fdtell(const int32_t fd) noexcept
		{ return _telli64(fd); }
	inline int32_t fdtruncate(const int32_t fd, const off_t size) noexcept
		{ return _chsize_s(fd, size); }
#endif

	struct fd_t final {
	private:
		constexpr static std::array<std::int8_t, 62> alphanum{
			/* Lowercase ASCII */
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
			'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
			/* Capital ASCII */
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
			'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			/* Decimal Digits */
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};

		[[nodiscard]]
		static std::string random_string(std::size_t len) noexcept {
			static std::mt19937 rand{std::random_device{}()};
			static std::uniform_int_distribution<std::size_t> rndchar(0, alphanum.size() - 1);

			std::string str(len, 0);
			std::generate_n(str.begin(), len, []() { return alphanum[rndchar(rand)]; });
			return str;
		}

		int32_t fd{-1};
		mutable bool eof{false};
		mutable off_t _length{-1};
		bool _temp{false};
		fs::path _filename{""sv};
	public:
		constexpr fd_t() = default;

		fd_t(const int32_t fd_) noexcept : fd{fd_} { }
		fd_t(const char *const file, const int flags, const mode_t mode = 0, bool temp = false) noexcept :
			fd{::open(file, flags, mode)}, _temp{temp}, _filename{file} { }
		fd_t(const std::string& file, const int flags, const mode_t mode = 0, bool temp = false) noexcept :
			fd{::open(file.c_str(), flags, mode)}, _temp{temp}, _filename{file} { }
		fd_t(const fs::path& file, const int flags, const mode_t mode = 0, bool temp = false) noexcept :
			fd{::open(file.c_str(), flags, mode)}, _temp{temp}, _filename{file} { }
		fd_t(fd_t &&fd_) noexcept : fd_t{} { swap(fd_); }
		~fd_t() noexcept {
			if (fd != -1) {
				close(fd);
				if (_temp) {
					fs::remove(_filename);
				}
			}
		}

		[[nodiscard]]
		static fd_t maketemp(const int flags, const mode_t mode = 0, const std::string_view& ext = ".tmp"sv) noexcept {
			auto filepath = fs::temp_directory_path() / fs::path{fd_t::random_string(16)};
			filepath  += ext;
			return fd_t{filepath, flags | O_CREAT, mode, true};
		}

		void operator =(fd_t &&fd_) noexcept { swap(fd_); }
		[[nodiscard]]
		operator int32_t() const noexcept { return fd; }
		[[nodiscard]]
		bool operator ==(const int32_t desc) const noexcept { return fd == desc; }
		[[nodiscard]]
		bool valid() const noexcept { return fd != -1; }
		[[nodiscard]]
		bool isEOF() const noexcept { return eof; }
		[[nodiscard]]
		fs::path filename() const noexcept { return _filename; }

		void invalidate() noexcept { fd = -1; }

		void swap(fd_t &desc) noexcept {
			std::swap(fd, desc.fd);
			std::swap(eof, desc.eof);
			std::swap(_length, desc._length);
			std::swap(_temp, desc._temp);
			std::swap(_filename, desc._filename);
		}

		[[nodiscard]]
		ssize_t read(void *const bufferPtr, const size_t bufferLen, std::nullptr_t) const noexcept {
			const auto result = internal::fdread(fd, bufferPtr, bufferLen);
			if (!result && bufferLen) {
				eof = true;
			}
			return result;
		}

		[[nodiscard]]
		off_t seek(const off_t offset, const int32_t whence = SEEK_CUR) const noexcept {
			const auto result = internal::fdseek(fd, offset, whence);
			eof = result == length();
			return result;
		}

		[[nodiscard]]
		ssize_t write(const void *const bufferPtr, const size_t bufferLen, std::nullptr_t) const noexcept
			{ return internal::fdwrite(fd, bufferPtr, bufferLen); }
		[[nodiscard]]
		off_t tell() const noexcept { return internal::fdtell(fd); }

		[[nodiscard]]
		bool head() const noexcept { return seek(0, SEEK_SET) == 0; }
		[[nodiscard]]
		fd_t dup() const noexcept { return ::dup(fd); }

		[[nodiscard]]
		bool tail() const noexcept {
			const auto offset = length();
			if (offset < 0) {
				return false;
			}
			return seek(offset, SEEK_SET) == offset;
		}

		[[nodiscard]]
		internal::stat_t stat() const noexcept {
			internal::stat_t fileStat{};
			if (!internal::fstat(fd, &fileStat)) {
				return fileStat;
			}
			return {};
		}

		[[nodiscard]]
		off_t length() const noexcept {
			if (_length != -1) {
				return _length;
			}
			internal::stat_t fileStat{};
			const int result = internal::fstat(fd, &fileStat);
			_length = result ? -1 : fileStat.st_size;
			return _length;
		}

		[[nodiscard]]
		bool resize(const off_t newSize) const noexcept
			{ return internal::fdtruncate(fd, newSize) == 0; }

		[[nodiscard]]
		bool read(void *const value, const size_t valueLen, size_t &resultLen) const noexcept {
			const ssize_t result = read(value, valueLen, nullptr);
			if (result < 0) {
				return false;
			}
			resultLen = size_t(result);
			return resultLen == valueLen;
		}

		[[nodiscard]]
		bool read(void *const value, const size_t valueLen) const noexcept {
			size_t resultLen = 0;
			return read(value, valueLen, resultLen);
		}

		[[nodiscard]]
		bool write(const void *const value, const size_t valueLen) const noexcept {
			const ssize_t result = write(value, valueLen, nullptr);
			if (result < 0) {
				return false;
			}
			return size_t(result) == valueLen;
		}

		template<typename T>
		[[nodiscard]]
		bool read(T &value) const noexcept
			{ return read(&value, sizeof(T)); }
		template<typename T>
		[[nodiscard]]
		bool write(const T &value) const noexcept
			{ return write(&value, sizeof(T)); }
		template<typename T>
		[[nodiscard]]
		bool read(std::unique_ptr<T> &value) const noexcept
			{ return read(value.get(), sizeof(T)); }
		template<typename T>
		[[nodiscard]]
		bool read(const std::unique_ptr<T> &value) const noexcept
			{ return read(value.get(), sizeof(T)); }
		template<typename T>
		[[nodiscard]]
		bool write(const std::unique_ptr<T> &value) const noexcept
			{ return write(value.get(), sizeof(T)); }
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
		template<typename T>
		[[nodiscard]]
		bool read(const std::unique_ptr<T []> &value, const size_t valueCount) const noexcept
			{ return read(value.get(), sizeof(T) * valueCount); }
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
		template<typename T>
		[[nodiscard]]
		bool write(const std::unique_ptr<T []> &value, const size_t valueCount) const noexcept
			{ return write(value.get(), sizeof(T) * valueCount); }
		template<typename T, size_t N>
		[[nodiscard]]
		bool read(std::array<T, N> &value) const noexcept
			{ return read(value.data(), sizeof(T) * N); }
		template<typename T, size_t N>
		[[nodiscard]]
		bool write(const std::array<T, N> &value) const noexcept
			{ return write(value.data(), sizeof(T) * N); }

		template<typename T>
		[[nodiscard]]
		std::optional<T> read() const noexcept {
			T local{};
			const auto res = read(local);
			return res ? std::optional<T>{local} : std::nullopt;
		}

		[[nodiscard]]
		bool write(const std::string &value) const noexcept
			{ return write(value.data(), value.size()); }

		[[nodiscard]]
		bool write(const std::string_view &value) const noexcept
			{ return write(value.data(), value.size()); }

		template<size_t length, typename T, size_t N>
		[[nodiscard]]
		bool read(std::array<T, N> &value) const noexcept {
			static_assert(length <= N, "Can't request to read more than the std::array<> length");
			return read(value.data(), sizeof(T) * length);
		}

		template<size_t length, typename T, size_t N>
		[[nodiscard]]
		bool write(const std::array<T, N> &value) const noexcept {
			static_assert(length <= N, "Can't request to write more than the std::array<> length");
			return write(value.data(), sizeof(T) * length);
		}

		[[nodiscard]]
		bool readLE(uint16_t &value) const noexcept {
			std::array<uint8_t, 2> data{};
			const bool result = read(data);
			value = (uint16_t(data[1]) << 8U) | data[0];
			return result;
		}

		[[nodiscard]]
		bool writeLE(const uint16_t value) const noexcept {
			const std::array<uint8_t, 2> data
			{{
				uint8_t(value),
				uint8_t(value >> 8U)
			}};
			return write(data);
		}

		[[nodiscard]]
		bool readLE(uint32_t &value) const noexcept {
			std::array<uint8_t, 4> data{};
			const bool result = read(data);
			value = (uint32_t(data[3]) << 24U) | (uint32_t(data[2]) << 16U) |
				(uint32_t(data[1]) << 8U) | data[0];
			return result;
		}

		[[nodiscard]]
		bool writeLE(const uint32_t value) const noexcept {
			const std::array<uint8_t, 4> data
			{{
				uint8_t(value),
				uint8_t(value >> 8U),
				uint8_t(value >> 16U),
				uint8_t(value >> 24U)
			}};
			return write(data);
		}

		[[nodiscard]]
		bool readLE(uint64_t &value) const noexcept {
			std::array<uint8_t, 8> data{};
			const bool result = read(data);
			value = (uint64_t(data[7]) << 56U) | (uint64_t(data[6]) << 48U) |
				(uint64_t(data[5]) << 40U) | (uint64_t(data[4]) << 32U) |
				(uint64_t(data[3]) << 24U) | (uint64_t(data[2]) << 16U) |
				(uint64_t(data[1]) << 8U) | data[0];
			return result;
		}

		[[nodiscard]]
		bool writeLE(const uint64_t value) const noexcept {
			const std::array<uint8_t, 8> data
			{{
				uint8_t(value),
				uint8_t(value >> 8U),
				uint8_t(value >> 16U),
				uint8_t(value >> 24U),
				uint8_t(value >> 32U),
				uint8_t(value >> 40U),
				uint8_t(value >> 48U),
				uint8_t(value >> 56U)
			}};
			return write(data);
		}

		template<typename T, typename = typename std::enable_if<
			std::is_integral<T>::value && !std::is_same<T, bool>::value &&
			std::is_signed<T>::value && sizeof(T) >= 2>::type
		>
		[[nodiscard]]
		bool readLE(T &value) const noexcept {
			typename std::make_unsigned<T>::type data{};
			const auto result = readLE(data);
			value = static_cast<T>(data);
			return result;
		}

		template<typename T, typename = typename std::enable_if<
			std::is_integral<T>::value && !std::is_same<T, bool>::value &&
			std::is_signed<T>::value && sizeof(T) >= 2>::type
		>
		[[nodiscard]]
		bool writeLE(const T value) const noexcept {
			auto data{static_cast<typename std::make_unsigned<T>::type>(value)};
			return writeLE(data);
		}

		[[nodiscard]]
		bool readBE(uint16_t &value) const noexcept {
			std::array<uint8_t, 2> data{};
			const bool result = read(data);
			value = (uint16_t(data[0]) << 8U) | data[1];
			return result;
		}

		[[nodiscard]]
		bool writeBE(const uint16_t value) const noexcept {
			const std::array<uint8_t, 2> data
			{{
				uint8_t(value >> 8U),
				uint8_t(value)
			}};
			return write(data);
		}

		[[nodiscard]]
		bool readBE(uint32_t &value) const noexcept {
			std::array<uint8_t, 4> data{};
			const bool result = read(data);
			value = (uint32_t(data[0]) << 24U) | (uint32_t(data[1]) << 16U) |
				(uint32_t(data[2]) << 8U) | data[3];
			return result;
		}

		[[nodiscard]]
		bool writeBE(const uint32_t value) const noexcept {
			const std::array<uint8_t, 4> data
			{{
				uint8_t(value >> 24U),
				uint8_t(value >> 16U),
				uint8_t(value >> 8U),
				uint8_t(value)
			}};
			return write(data);
		}

		[[nodiscard]]
		bool readBE(uint64_t &value) const noexcept {
			std::array<uint8_t, 8> data{};
			const bool result = read(data);
			value = (uint64_t(data[0]) << 56U) | (uint64_t(data[1]) << 48U) |
				(uint64_t(data[2]) << 40U) | (uint64_t(data[3]) << 32U) |
				(uint64_t(data[4]) << 24U) | (uint64_t(data[5]) << 16U) |
				(uint64_t(data[6]) << 8U) | data[7];
			return result;
		}

		[[nodiscard]]
		bool writeBE(const uint64_t value) const noexcept {
			const std::array<uint8_t, 8> data
			{{
				uint8_t(value >> 56U),
				uint8_t(value >> 48U),
				uint8_t(value >> 40U),
				uint8_t(value >> 32U),
				uint8_t(value >> 24U),
				uint8_t(value >> 16U),
				uint8_t(value >> 8U),
				uint8_t(value)
			}};
			return write(data);
		}

		template<typename T, typename = typename std::enable_if<
			std::is_integral<T>::value && !std::is_same<T, bool>::value &&
			std::is_signed<T>::value && sizeof(T) >= 2>::type
		>
		[[nodiscard]]
		bool readBE(T &value) const noexcept {
			typename std::make_unsigned<T>::type data{};
			const auto result = readBE(data);
			value = static_cast<T>(data);
			return result;
		}

		template<typename T, typename = typename std::enable_if<
			std::is_integral<T>::value && !std::is_same<T, bool>::value &&
			std::is_signed<T>::value && sizeof(T) >= 2>::type
		>
		[[nodiscard]]
		bool writeBE(const T value) const noexcept {
			auto data{static_cast<typename std::make_unsigned<T>::type>(value)};
			return writeBE(data);
		}

		[[nodiscard]]
		bool seekRel(const off_t offset) const noexcept {
			const off_t currentPos = tell();
			if (currentPos == -1 || currentPos + offset < 0)
				return false;
			return seek(offset, SEEK_CUR) == currentPos + offset;
		}

		fd_t(const fd_t &) = delete;
		fd_t &operator =(const fd_t &) = delete;
	};

	inline void swap(fd_t &a, fd_t &b) noexcept { a.swap(b); }
#ifndef _WINDOWS
	constexpr mode_t normalMode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; // NOLINT(hicpp-signed-bitwise)
#else
	constexpr mode_t normalMode = _S_IWRITE | _S_IREAD; // NOLINT(hicpp-signed-bitwise)
#endif

}

#endif /* LIBNOKOGIRI_INTERNAL_FD_HH */
