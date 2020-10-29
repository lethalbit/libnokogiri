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

	// enum struct zlib_mode_t : std::uint8_t {
	// 	Inflate,
	// 	Deflate,
	// };

	// template<zlib_mode_t mode>
	// struct LIBNOKOGIRI_CLS_API zlib_t final {
	// public:
	// 	constexpr static const auto chunk_length = 8_KiB;
	// private:
	// 	fd_t _file;
	// 	zlib_mode_t _mode{mode};
	// 	std::array<std::uint8_t, chunk_length> _input_buffer;
	// 	std::array<std::uint8_t, chunk_length> _output_buffer;
	// 	std::uint64_t _buffer_used;
	// 	z_stream _stream;
	// 	bool _eos;

	// 	bool init_zlib(const std::int32_t window_size) {
	// 		if constexpr (mode == zlib_mode_t::Inflate) {
	// 			return inflateInit2(&_stream, window_size) != Z_OK;
	// 		} else if constexpr (mode == zlib_mode_t::Deflate) {
	// 			return deflateInit2(&_stream, Z_BEST_COMPRESSION, Z_DEFLATED, window_size, 8, Z_DEFAULT_STRATEGY) != Z_OK;
	// 		}
	// 	}

	// public:
	// 	constexpr zlib_t() noexcept :
	// 		_file{}, _input_buffer{}, _output_buffer{},
	// 		_buffer_used{0U}, _stream{}, _eos{true}
	// 		{ /* NOP */ }

	// 	zlib_t(fd_t& file, std::int32_t window_size = MAX_WBITS + 16) noexcept :
	// 		_file{std::move(file)}, _input_buffer{}, _output_buffer{},
	// 		_buffer_used{0U}, _stream{}, _eos{true} {

	// 		if (!init_zlib(window_size)) {
	// 			_file = {};
	// 		}
	// 	}

	// 	zlib_t(const fs::path& file, std::int32_t flags = O_RDONLY, mode_t fmode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, std::int32_t window_size = MAX_WBITS + 16) noexcept :
	// 		_file{file, flags, fmode}, _input_buffer{}, _output_buffer{},
	// 		_buffer_used{0U}, _stream{}, _eos{true} {

	// 		if (!init_zlib(window_size)) {
	// 			_file = {};
	// 		}

	// 	}

	// 	zlib_t(zlib_t&&) = delete;
	// 	zlib_t& operator=(zlib_t&&) = delete;

	// 	template<typename T, std::size_t len>
	// 	std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Inflate>, bool>
	// 	decompress(std::array<T, len>& buffer) noexcept {

	// 	}

	// 	template<typename T>
	// 	std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, std::optional<T>>
	// 	decompress() noexcept {

	// 	};

	// 	template<typename T, std::size_t len>
	// 	std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, bool>
	// 	compress(std::array<T, len>& buffer) noexcept {

	// 	}

	// 	template<typename T>
	// 	std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, bool>
	// 	compress(T&);

	// 	[[nodiscard]]
	// 	bool eos() const noexcept { return _eos; }
	// };

	struct gzfile_t final {
		fd_t _file;
		std::unique_ptr<gzFile_s, decltype(&gzclose)> _gz_file;
		std::int32_t _error{Z_OK};
	public:
		gzfile_t() noexcept :
			_file{-1}, _gz_file{nullptr, nullptr}
			{ /* NOP */ }

		gzfile_t(fd_t& file) noexcept :
			_file{std::move(file)}, _gz_file(gzdopen(_file, "r+b"), gzclose)
			{ /* NOP */ }

		gzfile_t(const fs::path& file, std::int32_t flags = O_RDONLY, mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) noexcept :
			_file{file, flags, mode}, _gz_file(gzdopen(_file, "r+b"), gzclose)
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
			return (_error = gzread(_gz_file.get(), data.data(), len * sizeof(T))) == Z_OK;
		}

		template<typename T>
		[[nodiscard]]
		std::optional<T> read() noexcept {
			T local_storage{};
			_error = gzread(_gz_file.get(), &local_storage, sizeof(T));
			return (_error == Z_OK) ? std::optional<T>{local_storage} : std::nullopt;
		}

		template<typename T>
		[[nodiscard]]
		std::enable_if_t<std::is_integral_v<T>, std::optional<T>>
		bswap_read() noexcept {
			T local_storage{};
			_error = gzread(_gz_file.get(), &local_storage, sizeof(T));

			if constexpr (sizeof(T) == sizeof(std::uint16_t)) {
				local_storage = LIBNOKOGIRI_SWAP16(local_storage);
			} else if constexpr (sizeof(T) == sizeof(std::uint32_t)) {
				local_storage = LIBNOKOGIRI_SWAP32(local_storage);
			} else if constexpr (sizeof(T) == sizeof(std::uint64_t)) {
				local_storage = LIBNOKOGIRI_SWAP64(local_storage);
			}

			return (_error == Z_OK) ? std::optional<T>{local_storage} : std::nullopt;
		}

		template<typename T, std::size_t len>
		[[nodiscard]]
		bool write(const std::array<T, len>& data) noexcept {
			return (_error = gzwrite(_gz_file.get(), data.data(), len * sizeof(T))) == Z_OK;
		}

		template<typename T>
		[[nodiscard]]
		bool write(const T& data) noexcept {
			return (_error = gzwrite(_gz_file.get(), &data, sizeof(T))) == Z_OK;
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

			return (_error = gzwrite(_gz_file.get(), &local_storage, sizeof(T))) == Z_OK;
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
		const std::string last_error_str() noexcept { return std::string(gzerror(_gz_file.get(), &_error)); }

		[[nodiscard]]
		std::int32_t last_error() noexcept { return _error; }

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
