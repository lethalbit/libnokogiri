// SPDX-License-Identifier: LGPL-3.0-or-later
/* internal/fs.hh - C++ Filesystem library wrapper */
#pragma once
#if !defined(LIBNOKOGIRI_INTERNAL_FS_HH)
#define LIBNOKOGIRI_INTERNAL_FS_HH

#include <libnokogiri/config.hh>

#if defined(LIBNOKOGIRI_CPPFS_EXPERIMENTAL)
	#include <experimental/filesystem>
namespace libnokogiri::internal {
	namespace fs = std::experimental::filesystem;
}
#else
	#include <filesystem>
namespace libnokogiri::internal {
	namespace fs = std::filesystem;
}
#endif

#endif /* LIBNOKOGIRI_INTERNAL_FS_HH */
