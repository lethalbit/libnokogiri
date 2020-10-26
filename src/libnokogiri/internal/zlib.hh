// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/zlib.hh - libnokogiri zlib wrapper */
#if !defined(LIBNOKOGIRI_INTERNAL_ZLIB_HH)
#define LIBNOKOGIRI_INTERNAL_ZLIB_HH

#include <array>
#include <cstdint>

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

	enum struct zlib_mode_t : std::uint8_t {
		Inflate,
		Deflate,
	};

	template<zlib_mode_t mode>
	struct LIBNOKOGIRI_CLS_API zlib_t final {
	public:
		constexpr static const auto chunk_length = 8_KiB;
	private:
		fd_t _file;
		zlib_mode_t _mode{mode};
		std::array<std::uint8_t, chunk_length> _input_buffer;
		std::array<std::uint8_t, chunk_length> _output_buffer;
		std::uint64_t _buffer_used;
		z_stream _stream;
		bool _eos;

		bool init_zlib(const std::int32_t window_size) {
			if constexpr (mode == zlib_mode_t::Inflate) {
				return inflateInit2(&_stream, window_size) != Z_OK;
			} else if constexpr (mode == zlib_mode_t::Deflate) {
				return deflateInit2(&_stream, Z_BEST_COMPRESSION, Z_DEFLATED, window_size, 8, Z_DEFAULT_STRATEGY) != Z_OK;
			}
		}

	public:
		constexpr zlib_t() noexcept :
			_file{}, _input_buffer{}, _output_buffer{},
			_buffer_used{0U}, _stream{}, _eos{true}
			{ /* NOP */ }

		zlib_t(fd_t& file, std::int32_t window_size = MAX_WBITS + 16) noexcept :
			_file{file}, _input_buffer{}, _output_buffer{},
			_buffer_used{0U}, _stream{}, _eos{true} {

			if (!init_zlib(window_size)) {
				_file = {};
			}
		}

		zlib_t(fs::path& file, std::int32_t window_size = MAX_WBITS + 16) noexcept :
			_file{file}, _input_buffer{}, _output_buffer{},
			_buffer_used{0U}, _stream{}, _eos{true} {

			if (!init_zlib(window_size)) {
				_file = {};
			}

		}

		zlib_t(zlib_t&&) = delete;
		zlib_t& operator=(zlib_t&&) = delete;

		template<typename T, std::size_t len>
		std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Inflate>, bool>
		decompress(std::array<T, len>& buffer);

		template<typename T>
		std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, T>
		decompress();

		template<typename T, std::size_t len>
		std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, bool>
		compress(std::array<T, len>& buffer);

		template<typename T>
		std::enable_if_t<std::is_same_v<mode, zlib_mode_t::Deflate>, T>
		decompress(T&);

		[[nodiscard]]
		bool eos() const noexcept { return _eos; }
	};


	struct gzfile_t final {
	private:


	public:

	};
}

#endif /* LIBNOKOGIRI_INTERNAL_ZLIB_HH */
