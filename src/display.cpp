
#include <string.h>
#include <display.hpp>

namespace display {

tpl *
get(const byte type) {
#if OS == LINUX
	switch (type) {
	case WAYLAND: return reinterpret_cast<tpl *>(new wayland);
	case X11:	  return reinterpret_cast<tpl *>(new x11);
	default:	  return nullptr;
	}
#elif OS == OSX
	INFO(WARN"OSX casting isn't supported yet. If you need this "
			 "function - please open ticket on github page");
	return nullptr;
#elif OS == WIN
	return reinterpret_cast<tpl *>(new d3d11);
#else
	return nullptr;
#endif
}

void
pixs::next(void) {
	ptr += shift;
}

void
pixs::next(const size_t &num) {
	ptr += shift * num;
}

void
events::clear(void) {
	mouse = std::make_pair(uint16_t{0}, uint16_t{0});
	for (size_t i = 0; i < MAXKEYS; i++) {
		buttons[i] = { keys::types::NO_TYPE, uint32_t{0} }; 
	}
}

void
events::pack(byte *ptr) {
	*reinterpret_cast<uint16_t *>(ptr + 2) = mouse.second;
	*reinterpret_cast<uint16_t *>(ptr)     = mouse.first;
	ptr += 4;

	for (size_t i = 0; i < MAXKEYS; i++, ptr += 5) {
		*reinterpret_cast<uint32_t *>(ptr) = buttons[i].key;
		ptr[4] = buttons[i].type;
	}
}

void
events::set(const byte *ptr) {
	mouse.second = *reinterpret_cast<const uint16_t *>(ptr + 2);
	mouse.first  = *reinterpret_cast<const uint16_t *>(ptr);
	ptr += 4;

	for (size_t i = 0; i < MAXKEYS; i++, ptr += 5) {
		buttons[i].key = *reinterpret_cast<const uint32_t *>(ptr);
		buttons[i].type = static_cast<const keys::types>(ptr[4]);
	}
}

}