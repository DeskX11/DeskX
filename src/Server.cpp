
#include "../include/Server.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << readme << std::endl;
		return 0;
	}

	Server::args = Args(argc, argv);

	if (Server::args.get().port < 1) {
		std::cout << "Error: Invalid port number.\n";
		return 1;
	}

	std::string tmp = Server::args.get().xauth, cmd;
	Server::tcp = Net(Server::args.get().port,
					  Server::args.get().ip,  true);
	if (tmp.empty()) {
		char *env = getenv("XAUTHORITY");
		ERR(!env, "Argument `xauth` was not set");
		tmp = std::string(env);
	}
	putenv(strdup((std::string("XAUTHORITY=") + tmp).c_str()));

	tmp = Server::args.get().display;
	if (tmp.empty()) {
		char *env = getenv("DISPLAY");
		ERR(!env, "Argument `display` was not set");
		tmp = std::string(env);
	}
	putenv(strdup((std::string("DISPLAY=") + tmp).c_str()));

	while (Server::work) {
		NEXT_IF(!Server::tcp.accept());

		assert(Server::x11 = new X11);
		Actions::start();

		Server::tcp.close();
		delete Server::x11;
	}

	Server::tcp.closeAll();
}
