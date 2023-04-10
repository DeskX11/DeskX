
#include "../include/DeskX.h"

Args::Args(int argc, char **argv) {
	RET_IF(argc < 2 || !argv);

	for (int i = 1; i < argc; i++) {
		char *val = strchr(argv[i], '=');
		ERR(!val, "Incorrect argument " + std::string(argv[i]));
		ERR(!(++val), "Argument " + std::string(argv[i]) + " is empty");

		NEXT_IF(val - argv[i] <= 3);
		ERR(!read(argv[i], val), "Incorrect argument " + std::string(argv[i]));
	}
}

bool Args::read(char *full, char *val) {
	*(val - 1) = 0;
	std::string name(full + 2), value(val);
	bool status = false;

	if (name == "ip" || name == "bind-ip") {
		args.ip = value;
		status = true;
	}
	if (name == "port") {
		args.port = atoi(value.c_str());
		status = true;
	}
	if (name == "resolution") {
		unsigned int w = 0, h = 0;
		sscanf(value.c_str(), "%4ux%4u", &w, &h);
		args.mode.width  = w;
		args.mode.height = h;
		status = true;
	}
	if (name == "vertical-distance") {
		args.mode.vdistance = atoi(value.c_str());
		status = true;
	}
	if (name == "color-distance") {
		args.mode.hdistance = atoi(value.c_str());
		status = true;
	}
	if (name == "display") {
		args.display = value;
		status = true;
	}
	if (name == "xauth") {
		args.xauth = value;
		status = true;
	}
	if (name == "cmd") {
		unsigned int cmd;
		sscanf(value.c_str(), "%1u", &cmd);
		ERR(cmd > END, "Incorrect `cmd` argument");
		args.mode.func = static_cast<Func>(cmd);
		status = true;
	}

	return status;
}

void Args::operator=(const Args &inps) {
	args = inps.args;
}

byte *Args::pack(void) {
	byte *buff = new byte[Consts::req];
	byte *tmp  = buff;
	assert(buff);

	memcpy(tmp, &args.mode.func, 1);
	tmp++;
	memcpy(tmp, &args.mode.width, Consts::u16);
	tmp += Consts::u16;
	memcpy(tmp, &args.mode.height, Consts::u16);
	tmp += Consts::u16;
	*tmp++ = args.mode.hdistance;
	*tmp++ = args.mode.vdistance;
#if defined(__APPLE__) || defined(__CYGWIN__)
	args.mode.sdl = 1;
#endif
	*tmp = args.mode.sdl;

	return buff;
}

bool Args::unpack(byte *config) {
	uint8_t func;

	RET_IF(!config,    false);
	memcpy(&func, config,  1);
	RET_IF(func > END, false);
	args.mode.func = static_cast<Func>(func);
	config++;

	memcpy(&args.mode.width, config, Consts::u16);
	config += Consts::u16;
	memcpy(&args.mode.height, config, Consts::u16);
	config += Consts::u16;
	args.mode.hdistance = *config++;
	args.mode.vdistance = *config++;
	args.mode.sdl = *config;

	return true;
}

Inputs &Args::get(void) {
	return args;
}