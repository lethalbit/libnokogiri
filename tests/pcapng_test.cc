// SPDX-License-Identifier: LGPL-3.0-or-later
/* pcapng_test.cc - libnokogiri test harness for pcapng files */

#include <iostream>
#include <cstring>

#include <libnokogiri/pcapng.hh>

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

	return {};
}

int write(fs::path in, fs::path out) {

	return {};
}
