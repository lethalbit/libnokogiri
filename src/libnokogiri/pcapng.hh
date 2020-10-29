// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng.hh - pcapng file format interface for libnokogiri */
#if !defined(LIBNOKOGIRI_PCAPNG_HH)
#define LIBNOKOGIRI_PCAPNG_HH

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>

#include <libnokogiri/config.hh>
#include <libnokogiri/common.hh>

#include <libnokogiri/internal/defs.hh>
#include <libnokogiri/internal/fs.hh>
#include <libnokogiri/internal/zlib.hh>
#include <libnokogiri/internal/file_wrapper.hh>

#include <libnokogiri/pcapng/block.hh>
#include <libnokogiri/pcapng/blocks.hh>

#include <libnokogiri/pcapng/option.hh>
#include <libnokogiri/pcapng/options.hh>

#include <libnokogiri/pcapng/section.hh>

namespace libnokogiri::pcapng {

	/*! \struct libnokogiri::pcapng::pcapng_t
		\brief pcapng file container

		The working draft for the pcapng format is specified in [pcapng/pcapng](https://github.com/pcapng/pcapng) repo.

		This structure contains the machinery to read, write, and edit pcapng files.
	*/
	struct pcapng_t final {
	public:

	};
}

#endif /* LIBNOKOGIRI_PCAPNG_HH */
