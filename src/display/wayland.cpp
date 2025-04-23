
#include <display.hpp>

namespace display {

bool
wayland::init(void) {
	return false;
}

void
wayland::close(void) {
	return;
}

xy
wayland::res(void) {
	return std::make_pair(0, 0);
}

void
wayland::refresh(pixs &arg) {
	return;
}

void
wayland::set(const events &arg) {

}

}