
#include <macro.hpp>
#include <args.hpp>
#include <client.hpp>
#include <server.hpp>

std::string
usage(const int num) {
	const std::string sopts = "Server's options:\n\t--bind-ip\t\tIP address to listen "
							  "on (default: All)\n\t--port\t\t\tConnection port\n";
	const std::string copts = "Client's options:\n\t--ip\t\t\tIP address of the server\n"
							  "\t--port\t\t\tPort of the server\n\t--color-distance\t"
							  "Compression range (1-255) (default: 2)\n\t--fps\t\t\t"
							  "Frame limit (default: 50)\n";
	switch (num) {
	case 1:	 return "Usage: ./deskx client [options]\n" + copts;
	case 2:	 return "Usage: ./deskx server [options]\n" + sopts;
	default: return "Usage: ./deskx [mode] [options]\nModes:\n\tclient\t\t\tMode for "
					"controlling a remote computer\n\tserver\t\t\tMode for the computer"
					" to be controlled\n\n" + copts + "\n" + sopts + "\nExample:\n\t"
					"./deskx client --ip=192.168.0.1 --port=1742 --color-distance=2\n";
	}
}

int
main(int argc, char *argv[]) {
	const args args(argc, argv);
	if (args.ok()) {
		return args.mode() == args::type::CLIENT ? client::start(args)
												 : server::start(args);
	}
	switch (args.mode()) {
	case args::type::CLIENT: INFO(usage(1));
							 return 2;
	case args::type::SERVER: INFO(usage(2));
							 return 2;
	default:				 INFO(usage(0));
							 return 2;
	}
}