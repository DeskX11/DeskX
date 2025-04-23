
#ifndef DESKX_DISPLAY_WIN_HPP
#define DESKX_DISPLAY_WIN_HPP

namespace display {

class win : public tpl {
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