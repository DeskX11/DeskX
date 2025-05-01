
#include <display.hpp>
#include <display/osx/osxcast.h>

namespace display {

bool
osx::init(void) {
	return false;
}

void
osx::close(void) {
	return;
}

xy
osx::res(void) {
	return std::make_pair(0, 0);
}

void
osx::refresh(pixs &arg) {
	return;
}

void
osx::set(const events &arg) {

}

}