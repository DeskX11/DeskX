
#ifndef _DESKX_NETWORK_UDP_H_
#define _DESKX_NETWORK_UDP_H_

#define UDP_ATTEMPTS (uint8_t)20

class UDP {
private:
	int sock, packlim, userlim;
	bool sender = false;
	sddr_struct own, out;
	uint8_t pid = 1;

	bool BufferCheck(byte *, size_t);
	int Send(byte *, int);
	int Recv(byte *, int);

public:
	UDP(int, int, std::string ip = "");
	int Size(void) { return userlim; }
	int SendV(byte *);
	int RecvV(byte *);
	~UDP(void);
};

#endif