
#include "localdesk.h"

std::string man_text("\033[1mlocalDeskX\033[0m - Program for remote"\
					 "control of a computer in a local network. "	\
					 "(alpha)\nUsage: ldxc [Ip] [Port] [Password]"	\
					 " [Function] [Mode] [Range]\n\n"				\
					 "[Ip]\t   - Server ip address.\n"				\
					 "[Port]\t   - Server port number.\n"			\
					 "[Password] - Secret word without"				\
					 " spaces.\n[Function] - Number of one"			\
					 " of the listed functions:\n\t" 				\
					 "0  : Terminate server side process.\n\t"		\
					 "1  : Connect to the server and start "		\
					 "managing\n\t2  : Get a screenshot of "		\
					 "the desktop.\n[Mode]\t   - 0 when launched"	\
					 " in window mode, 1 when full window "			\
					 "mode.\n[Range]\t   - Compression range "		\
					 "(0 to 255).");
tcp_net *net;

void recv_screenshot(int argc, char **argv,
					 byte *req) {
	
}

void start_streaming(int argc, char **argv,
					 byte *req) {
	if (argc != 7) {
		std::cout << man_text << "\n";
		return;
	}

	req[1 + MD5S] = (uint8_t)atoi(argv[6]);
	req[0] = 0x01;

	assert(net->send_data(req, RSIZE));

	headers hdrs;
	byte hbuff[hdrs.size];
	memset(hbuff, 0, hdrs.size);

	if (!net->recv_data(hbuff, hdrs.size)) {
		std::cout << "Error: Incorrect headers"
				  << " or password.\n";
		return;
	}

	hdrs.from(hbuff);
	x11_client x11(hdrs, argv[5][0] != '0');

	size_t size = hdrs.width * hdrs.height
				* BSIZE;
	byte *buff = new byte[size];
	uint32_t blen;
	byte *bp = reinterpret_cast<byte *>(&blen);

	assert(buff);

	for (;;) {
		BREAK_IF(!net->recv_data(bp, U32S));
		BREAK_IF(blen == 0);

		size_t rs = blen * BSIZE;
		BREAK_IF(!net->recv_data(buff, rs));
		x11.set_pixels(buff, blen);

		// mouse & keys
		size_t evsize = x11.get_events(buff);
		evsize = evsize * 2 + U16S * 2 + 1;

		BREAK_IF(!net->send_data(buff, evsize));
	}

	delete[] buff;
}

int main(int argc, char *argv[]) {
	byte req[RSIZE];
	byte hash[MD5S];

	if (argc < 5) {
		std::cout << man_text << "\n";
		return 1;
	}

	std::string ip (argv[1]);
	int port = atoi(argv[2]);

	assert(net = new tcp_net(ip, port));
	
	MD5((byte *)argv[3], strlen(argv[3]), hash);
	memset(req, 0, RSIZE);
	memcpy(req + 1, hash, MD5S);

	switch (*argv[4]) {
	case '0':
		net->send_data(req, RSIZE);
		break;

	case '1':
		start_streaming(argc, argv, req);
		break;

	case '2':
		recv_screenshot(argc, argv, req);
	default: break;
	}

	delete net;
	return 0;
}

// just debug function.
void write_data(byte *buff, uint32_t blocks) {
	std::ofstream fs("./out.txt");

	assert(fs.is_open());
	fs << "Blocks: " << blocks << "\n";

	for (unsigned int i = 0; i < blocks; i++) {
		size_t shift = i * BSIZE;
		uint8_t len;
		uint8_t r, g, b;

		memcpy(&len, buff + shift, U8TS);
		r = buff[shift + U8TS];
		g = buff[shift + U8TS + 1];
		b = buff[shift + U8TS + 2];
		fs << "rgb(" << static_cast<int>(r) << ", "
		   << static_cast<int>(g) << ", "
		   << static_cast<int>(b) << "): "
		   << len << "\n";
	}

	fs.close();
}
