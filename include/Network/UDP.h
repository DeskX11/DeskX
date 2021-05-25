
#ifndef _DESKX_NETWORK_UDP_H_
#define _DESKX_NETWORK_UDP_H_

#define UDP_ATTEMPTS (uint8_t)8

class UDP {
private:
	bool sender = false;
	sddr_struct own, out;
	int sock, packlim;

	bool BufferCheck(size_t);
	int Send(byte *, int);
	int Recv(byte *, int);

public:
	UDP(int, int, std::string ip = "");
	int Size(void) { return packlim; }
	int SendV(byte *);
	int RecvV(byte *);
	~UDP(void);
};

#endif