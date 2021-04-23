
#include "localdesk.h"

std::string man_text("\033[1mlocalDeskX\033[0m - Program for remote"\
					 "control of a computer in a local network. "	\
					 "(alpha)\nUsage: ldxs [Port] [Password]\n\n"	\
					 "[Port]\t   - Connection port.\n"				\
					 "[Password] - Secret word without spaces.");
x11_server *x11;
tcp_net *net;

int pack_pixs(byte *buff, std::vector<pix> &vec) {
	uint32_t size = vec.size();
	memcpy(buff, &size, U32S);

	byte *ptr = buff + U32S;

	for (size_t i = 0; i < size; i++) {
		memcpy(ptr + i * BSIZE, &vec[i].num, U16S);
		ptr[i * BSIZE + U16S + 0] = vec[i].r;
		ptr[i * BSIZE + U16S + 1] = vec[i].g;
		ptr[i * BSIZE + U16S + 2] = vec[i].b;
	}

	return size * BSIZE + U32S;
}

void start_streaming(uint8_t compression) {
	assert(x11 = new x11_server(compression));

	size_t maxb = U32S + BSIZE * x11->pixs_num();
	headers hdrs = x11->get_headers();
	byte *buff = new byte[maxb];
	std::vector<pix> vec;
	uint16_t x, y;

	assert (buff);
	hdrs.to(buff);
	RET_IF_VOID(!net->send_data(buff, hdrs.size));

	for (;;) {
		x11->pixels_vector(vec);
		int size = pack_pixs(buff, vec);

		BREAK_IF(!net->send_data(buff, size ));
		BREAK_IF(!net->recv_data(buff, MSIZE));

		memcpy(&x, buff, U16S);
		memcpy(&y, buff + U16S, U16S);
		uint8_t evnum = buff[U16S * 2];

		x11->set_mouse(x, y);
		if (evnum != 0) {
			evnum *= 2;
			BREAK_IF(!net->recv_data(buff, evnum));
			x11->set_events(buff, evnum / 2);
		}

		vec.clear();
	}

	delete[] buff;
}

void send_screenshot(uint8_t compression) {
	/*assert(x11 = new x11_server(compression));

	headers hdrs = x11->get_headers();
	std::vector<pix> vec;

	x11->pixels_vector(vec);
	int size = pack_pixs(buff + hdrs.size, vec);
	byte buff[size];

	hdrs.to(buff);

	net->send_data(buff, size + hdrs.size);*/
}

void request_processing(byte *hash) {
	constexpr size_t size = MD5S + U8TS * 2;
	byte buff[size];

	RET_IF_VOID(!net->recv_data(buff, size));

	int cmp = memcmp(hash, buff + 1, MD5S);
	RET_IF_VOID(hash && cmp != 0);

	uint8_t compression = buff[1 + MD5S];

	switch (*buff) {
	case 0x00:
		net->stop();
		return;

	case 0x01:
		start_streaming(compression);
		delete x11;
		return;

	case 0x02:
		send_screenshot(compression);
		delete x11;

	default: return;
	}
}

int main(int argc, char *argv[]) {
	byte hash[MD5S];
	int port;

	if (argc < 3 || (port = atoi(argv[1])) < 0) {
		std::cout << man_text << "\n";
		return 1;
	}

	assert(net = new tcp_net(port));
	MD5((byte *)argv[2], strlen(argv[2]), hash);

	while (net->work()) {
		if (net->accept_connection()) {
			request_processing(hash);
			net->close_connection();
		}
	}

	delete net;
	return 0;
}


