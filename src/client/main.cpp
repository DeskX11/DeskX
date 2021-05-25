
#include "../../include/Client.h"

int main(int argc, char *argv[]) {
	Global::args = Tools::ArgsRead(argc, argv);
	byte hash[MD5_DIGEST_LENGTH];
	byte request[AUTH_SIZE];

	if (Global::args.ip.empty() || Global::args.port < 1) {
		std::cout << man_text << "\n";
		return 1;
	}

	byte *pass = (byte *)Global::args.pass.c_str();
	MD5(pass, Global::args.pass.length(), hash);

	Global::net = new TCP(Global::args.port, Global::args.ip);
	assert(Global::net);

	memcpy(request + 1, hash, MD5_DIGEST_LENGTH);

	switch (*Global::args.cmd.c_str()) {
	case 'r':
		/**
		 *	Remote control mode
		 */
		Actions::StartStreaming(request);
		break;

	case 'e':
		/**
		 *	Completion of work
		 */
		*request = 2;
		Global::net->Send(request, AUTH_SIZE);
		break;

	case 's':
		/**
		 *	Server screenshot capture mode
		 */
		Actions::ScreenShot(request);
	default: break;
	}

	delete Global::net;
	return 0;
}
