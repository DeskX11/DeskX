
#ifndef _DESKX_ARGS_H_
#define _DESKX_ARGS_H_

enum Func {
	RAT = 0, END = 1/*, SHOT = 1, XYLIST = 2, END = 3*/
};

struct Settings {
	uint16_t width = 0, height = 0;
	uint8_t hdistance = 2;
	uint8_t vdistance = 2;
	uint8_t sdl = 0;
	Func func = RAT; 
};

struct Inputs {
	std::string ip = "", display = "", xauth = "";
	Settings mode;
	int port;
};

class Args {
protected:
	Inputs args;

	bool read(char *, char *);

public:
	Args(void) { }
	byte *pack(void);
	bool unpack(byte *);
	Args(int, char **);
	Inputs &get(void);
	void operator=(const Args &);
};

#endif