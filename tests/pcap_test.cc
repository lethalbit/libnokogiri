// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcap_test.cc - libnokogiri test harness for pcap files */

#include <iostream>
#include <string>
#include <cstring>

#include <libnokogiri/pcap.hh>

#include <libnokogiri/internal/fs.hh>

namespace fs = libnokogiri::internal::fs;

int read(fs::path file);
int write(fs::path in, fs::path out);


int main(int argc, char** argv) {
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " [-r|-w] input file [output file (if -w is specified)]" << std::endl;
		return 1;
	}

	if (std::strncmp(argv[1], "-r", 2) == 0) {
		return read(fs::path{argv[2]});
	}

	if (std::strncmp(argv[1], "-w", 2) == 0) {
		return write(fs::path{argv[2]}, fs::path{argv[3]});
	}

	return 1;
}



int read(fs::path file) {
	if (!fs::exists(file) || !fs::is_regular_file(file)) {
		return 1;
	}

	libnokogiri::pcap::pcap_t capture{file, libnokogiri::captrue_compression_t::Autodetect, true};

	if (!capture.valid()) {
		return 1;
	}


	return {};
}

int write(fs::path in, fs::path out) {
	if (!fs::exists(in) || !fs::is_regular_file(in)) {
		return 1;
	}

	if (!fs::is_directory(out)) {
		return 1;
	}

	return {};
}
