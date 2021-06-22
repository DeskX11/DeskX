
#ifndef _DESKX_NETWORK_UDP_H_
#define _DESKX_NETWORK_UDP_H_

#define UDP_ATTEMPTS (uint8_t)8
#define UDP_MTU 1500

using namespace std::chrono;
using std::chrono::system_clock;

class UDP {
private:
	int sock, packlim, userlim;
	sddr_struct own, out;
	bool sender = false, *w;
	timeval tout = {0, 0};
	byte *pack, *back;
	uint8_t pid = 0;
	char *toptr;

	void RecvTimeout(long, bool a = false);
	bool CheckBuffer(byte *);
	void ClearBuffer(void);
	int Send(byte *, int);
	int Recv(byte *, int);

public:
	UDP(bool *, int, std::string ip = "");
	int Size(void) { return userlim; }
	int SendV(byte *);
	int RecvV(byte *);
	~UDP(void);
};

#endif