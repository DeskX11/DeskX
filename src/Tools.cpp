
#include "../include/DeskX.h"

void Tools::SharedMem(XShmSegmentInfo &shm, char **ptr, int length) {
	shm.shmid = shmget(IPC_PRIVATE, length * 2, IPC_CREAT | 0777);
	assert(shm.shmid > 0);

	*ptr = (char *)shmat(shm.shmid, 0, 0);
	shm.readOnly = false;
	shm.shmaddr  = *ptr;
	assert(shm.shmaddr != (char *)-1);

	shmctl(shm.shmid, IPC_RMID, 0);
}

uint16_t Tools::FreePort(uint16_t start) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	sddr_struct sddr;
	uint16_t port;

	assert(sock > -1);

	for (port = start; port != 0; port++) {
		Tools::Sddr(sddr, port);
		BREAK_IF(bind(sock, sddr.ptr, SDDR_SIZE) == 0);
	}

	Tools::RmSocket(sock);
	
	return port;
}

void Tools::RmSocket(int fd) {
	if (fd > -1) {
		shutdown(fd, SHUT_RDWR);
		close(fd);
	}
}

void Tools::Sddr(sddr_struct &s, int port, std::string ip) {
	s.sddr.sin_addr.s_addr = (!ip.empty()) ? inet_addr(ip.c_str())
										   : INADDR_ANY;
	s.sddr.sin_port = htons(port);
	s.sddr.sin_family = AF_INET;
}

input Tools::ArgsRead(int argc, char **argv) {
	struct arg {
		std::string inp; int min;
	};

	std::vector<arg> l = {
		{"--compression=",	15},
		{"--password=",		12},
		{"--encryption",	12},
		{"--port=",			 8},
		{"--cmd=",			 9},
		{"--ip=",			 8},
		{"--events=",		12},
		{"--screen=",		12}
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
		if (cmp(l[0], argv[i], &ptr)) {
			retval.comp = (uint8_t)atoi(ptr);
		}

		else if (cmp(l[1], argv[i], &ptr)) {
			retval.pass = std::string(ptr);
		}

		else if (cmp(l[2], argv[i], &ptr)) {
			retval.secure = true;
		}

		else if (cmp(l[3], argv[i], &ptr)) {
			retval.port = atoi(ptr);
		}

		else if (cmp(l[4], argv[i], &ptr)) {
			retval.cmd = std::string(ptr);
		}

		else if (cmp(l[5], argv[i], &ptr)) {
			retval.ip = std::string(ptr);
		}

		else if (cmp(l[6], argv[i], &ptr)) {
			retval.events = std::string(ptr);
		}

		else if (cmp(l[7], argv[i], &ptr)) {
			retval.screen = std::string(ptr);
		}
	}

	return retval;
}

void Tools::SetOpts(int sock) {
	struct timeval tv;
	int re = 1;

	char *opt = reinterpret_cast<char *>(&tv);
	tv.tv_sec  = 6;
	tv.tv_usec = 0;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,  opt, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,  opt, sizeof(tv));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re));
}

void Tools::SockBuffer(int sock, uint64_t size) {
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&size, U64S);
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&size, U64S);
}

size_t Tools::MTU(void) { return 1500; }
