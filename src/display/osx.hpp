
#ifndef DESKX_DISPLAY_OSX_HPP
#define DESKX_DISPLAY_OSX_HPP

namespace display {

class osx : public tpl {
public:
	bool
	init(void);

	void
	close(void);

	xy
	res(void);

	void
	refresh(pixs &);

	void
	set(const events &);
};

}

#endif