
#ifndef _DESKX_TOOLS_H_
#define _DESKX_TOOLS_H_

namespace Tools {
	void Sddr(sddr_struct &, int, std::string s = "");
	input ArgsRead(int, char **);
	uint16_t FreePort(uint16_t);
	void RmSocket(int &);
	void SetDisplay(std::string);
	void SetXAuth(std::string);
#ifndef __APPLE__
	void SharedMem(XShmSegmentInfo &, char **, int);
#endif
}

#endif