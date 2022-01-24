
#include "../include/Client.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << readme << std::endl;
		return 0;
	}

	Client::args = Args(argc, argv);

	switch (Actions::sync()) {
	case 1:
		std::cout << "Error: IP address or port is set incorrectly.\n";
		return 1;
	case 2:
		std::cout << "Error: Unable to connect to remote server.\n";
		Client::tcp.closeAll();
		return 1;
	case 3:
		std::cout << "Error: Unable to send configuration to server.\n";
		Client::tcp.closeAll();
		return 1;
	case 4:
		std::cout << "Error: Server didn't send available resolution.\n";
		Client::tcp.closeAll();
		return 1;
	case 5:
		std::cout << "Error: Args passed to the server are invalid.\n";
		Client::tcp.closeAll();
		return 1;
	case 6:
		std::cout << "Done: The request has been sent to the server.\n";
		Client::tcp.closeAll();
		return 0;
	default:
		Actions::start();
		Client::tcp.closeAll();
		return 0;
	}
}