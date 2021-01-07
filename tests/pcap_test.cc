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
		std::cerr << "Unable to find file " << file << '\n';
	}

	libnokogiri::pcap::pcap_t capture{file, libnokogiri::capture_compression_t::Autodetect, true};

	if (!capture.valid()) {
		std::cerr << "Capture file " << file << " is not valid \n";
		return 1;
	}

	auto& hdr = capture.header();

	std::cout << "File Type: " << libnokogiri::internal::enum_name(
			libnokogiri::pcap::pcap_variant_s, hdr.variant()
		) << '\n';
	std::cout << "Compression: " << libnokogiri::internal::enum_name(
			libnokogiri::capture_compression_s, capture.compression_type()
		) << '\n';
	std::cout << "Packet count: " << capture.packet_count() << '\n';

	std::cout << "Version: " << hdr.version().major_version() << hdr.version().minor_version() << '\n';
	std::cout << "TZ Offset: " << hdr.timezone_offset() << '\n';
	std::cout << "TS Accuracy: " << hdr.timestamp_accuracy() << '\n';
	std::cout << "Max Packet Len: " << hdr.max_packet_length() << '\n';
	std::cout << "Link Type: " << libnokogiri::internal::enum_name(
			libnokogiri::link_type_s, hdr.link_type()
		) << '\n';

	for (auto pkt : capture) {
		if (!pkt) {
			return 1;
		}

		auto& packet = pkt->get();

		if (packet.length() == 0) {
			return 1;
		}


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
