
#include <display.hpp>

namespace display {

bool
win::init(void) {
	return false;
}

void
win::close(void) {
	return;
}

xy
win::res(void) {
	return std::make_pair(0, 0);
}

void
win::refresh(pixs &arg) {
	return;
}

void
win::set(const events &arg) {

}

}