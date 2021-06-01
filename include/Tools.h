
#ifndef _DESKX_TOOLS_H_
#define _DESKX_TOOLS_H_

namespace Tools {
	void Sddr(sddr_struct &, int, std::string s = "");
	void SharedMem(XShmSegmentInfo &, char **, int);
	input ArgsRead(int, char **);
	uint16_t FreePort(uint16_t);
	void SockBuffer(int, uint64_t);
	void RmSocket(int &);
	void SetOpts(int);
	size_t MTU(void);
}

#endif