
#ifndef _DESKX_NETWORK_TCP_H_
#define _DESKX_NETWORK_TCP_H_

class TCP {
private:
	bool init = false, sender;
	int sock1, sock2;
	sddr_struct own, out;

public:
	TCP(int, std::string ip = "");
	bool Recv(byte *, uint32_t);
	bool Send(byte *, uint32_t);
	bool Accept(void);
	bool Alive(void) { return init;  }
	bool Exit(void)  { init = false; }
	void CloseConnection(void);
	void BufferSize(uint64_t);
	std::string GetIp(void);
	~TCP(void);
};

#endif
