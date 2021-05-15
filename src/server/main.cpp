
#include "deskx.h"
netw *net;

int pack_pixs(byte *buff, std::vector<pix> &vec) {
	uint32_t blockn = 0, linkn = 0, eq = 0, size;
	byte *ptr = buff + U32S;

	for (auto &p : vec) {
		if (p.eq) {
			ptr[0] = 0xff;
			memcpy(ptr + 1, &p.eqn, U32S);
			ptr += ESIZE;
			eq++;
			continue;
		}

		if (p.link) {
			ptr[0] = 0;
			ptr[1] = p.num;
			ptr[2] = p.link_id;
			ptr += 3;
			linkn++;
			continue;
		}

		ptr[0] = p.num;
		ptr[1] = p.r;
		ptr[2] = p.g;
		ptr[3] = p.b;
		blockn++;
		ptr += BSIZE;
	}

	size = blockn * BSIZE + linkn * 3 + eq * ESIZE;
	memcpy(buff, &size, U32S);

	return size + U32S;
}

void start_streaming(uint8_t compression) {
	x11_server x11(compression);

	size_t size = U32S + BSIZE * x11.pixs_num();
	headers hdrs = x11.get_headers();
	byte *buff = new byte[size];
	std::vector<pix> vec = { };
	uint16_t x, y;

	assert (buff);
	hdrs.to(buff);
	RET_IF_VOID(!net->send_data(buff, hdrs.size));

	x11.links_table(buff, size);
	RET_IF_VOID(!net->send_data(buff, size));

	for (;;) {
		x11.pixels_vector(vec);
		size = pack_pixs(buff, vec);

		BREAK_IF(!net->send_data(buff, size ));
		BREAK_IF(!net->recv_data(buff, MSIZE));

		memcpy(&x, buff, U16S);
		memcpy(&y, buff + U16S, U16S);
		uint8_t evnum = buff[U16S * 2];

		x11.set_mouse(x, y);
		if (evnum != 0) {
			evnum *= 2;
			BREAK_IF(!net->recv_data(buff, evnum));
			x11.set_events(buff, evnum / 2);
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
		//delete x11;
		return;

	case 0x02:
		send_screenshot(compression);
		//delete x11;

	default: return;
	}
}

input args_read(int argc, char *argv[]) {
	struct arg {
		std::string inp; int min;
	};

	std::vector<arg> l = {
		{"--password=",		12},
		{"--udp-port=",		12},
		{"--port=",			 8},
	};

	auto cmp = [&](arg &a, char *str, char **p) {
		int l1 = strlen(str);
		int l2 = a.inp.length();

		int cmp = strncmp(a.inp.c_str(), str, l2);
		*p = str + l2;

		return cmp == 0 && l1 >= a.min;
	};

	input retval;
	char *ptr;

	for (int i = 1; i < argc; i++) {
		if (cmp(l[2], argv[i], &ptr)) {
			retval.tcpport = atoi(ptr);
		}

		else if (cmp(l[0], argv[i], &ptr)) {
			retval.pass = std::string(ptr);
		}
	}

	return retval;
}

int main(int argc, char *argv[]) {
	byte hash[MD5S];
	input args = args_read(argc, argv);

	if (args.tcpport < 1 || args.pass.empty()) {
		std::cout << man_text << "\n";
		return 1;
	}

	byte *bpass = (byte *)args.pass.c_str();
	net = new netw(args.tcpport);

	MD5(bpass, args.pass.length(), hash);
	assert(net);

	while (net->work()) {
		if (net->accept_connection()) {
			request_processing(hash);
			net->close_connection ();
		}
	}

	delete net;
	return 0;
}


