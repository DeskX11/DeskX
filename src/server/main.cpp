
#include "../../include/Server.h"

void Processing(byte *hash) {
	byte req[AUTH_SIZE], res = 0;

	if (!Global::net->Recv(req, AUTH_SIZE)) {
		res = 1;
	}
	if (res == 0) {
		int cmp = memcmp(hash, req + 1, MD5_DIGEST_LENGTH);
		res = (cmp == 0) ? ((*req < 3) ? 0 : 3) : 2;
	}

	RET_IF_VOID(!Global::net->Send(&res, 1) || res != 0);
	Global::args.ip = Global::net->GetIp();

	switch (*req) {
	case 0:
		/**
		 *	Remote control mode
		 */
		Actions::StartStream(req[MD5_DIGEST_LENGTH + 1]);
		delete Global::x11;
		break;

	case 1:
		/**
		 *	Server screenshot capture mode
		 */
		Actions::ScreenShot(req[MD5_DIGEST_LENGTH  + 1]);
		delete Global::x11;
		break;

	default:
		/**
		 *	Completion of work
		 */
		Global::net->Exit();
	}
}

int main(int argc, char *argv[]) {
	Global::args = Tools::ArgsRead(argc, argv);
	byte hash[MD5_DIGEST_LENGTH];

	if (Global::args.port < 1 || Global::args.pass.empty()) {
		std::cout << man_text << "\n";
		return 1;
	}

	Global::net = new TCP(Global::args.port);
	assert(Global::net);

	byte *pass = (byte *)Global::args.pass.c_str();
	MD5(pass, Global::args.pass.length(), hash);

	while (Global::net->Alive()) {
		NEXT_IF(!Global::net->Accept());
		Processing(hash);

		Global::net->CloseConnection();
	}

	delete Global::net;
	return 0;
}