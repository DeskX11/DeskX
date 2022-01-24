
#ifndef _DESKX_CLIENT_ACTIONS_H_
#define _DESKX_CLIENT_ACTIONS_H_

class Actions {
protected:
	static void events(void);
	static void screen(void);

public:
	static uint8_t sync(void);
	static void start(void);

};

#endif
