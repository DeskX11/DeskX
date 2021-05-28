
#ifndef _DESKX_CLIENT_ACTIONS_H_
#define _DESKX_CLIENT_ACTIONS_H_

class Actions {
private:
	static void ProtsSync(uint16_t &, uint16_t &);
	static void Authorization(byte *, bool, bool, uint8_t);
	static void GetHeaders(void);

	static void EventsUDP(uint16_t, bool *);
	static void EventsTCP(uint16_t, bool *);
	static void ScreenUDP(uint16_t, bool &);
	static void ScreenTCP(bool &);

public:
	static void StartStreaming(byte *);
	static void ScreenShot(byte *);
};

#endif