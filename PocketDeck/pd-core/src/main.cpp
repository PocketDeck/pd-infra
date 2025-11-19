#include <exception>
#include <iostream>
#include <limits>
#include "server.hpp"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Invalid arguments!" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		return 1;
	}
	unsigned long port = std::stoul(argv[1]);
	if (port > std::numeric_limits<unsigned short>::max()) {
		std::cerr << "Invalid port number!" << std::endl;
		return 2;
	}

	try {
		boost::asio::io_context io;
		cards::server {io, static_cast<unsigned short>(port)};
	} catch (const std::exception& e) {
		return 1;
	}
	return 0;
}
