
#include "../include/DeskX.h"

size_t Events::need(void) const {
	return number * Consts::key + Consts::u16 * 2;
}

void Events::decode(byte *package) {
	RETVOID_IF(!package);

	list.clear();

	memcpy(&y, package + Consts::u16, Consts::u16);
	memcpy(&x, package, Consts::u16);
	RETVOID_IF(!number);
	package += Consts::u16 * 2;

	std::pair<uint32_t, KeyType> pair;

	for (int i = 0; i < number; i++) {
		byte *tmp = package;
		package += Consts::key;

		uint8_t type = tmp[Consts::u32];
		NEXT_IF(type > KEY_DOWN);

		memcpy(&pair.first, tmp, Consts::u32);
		pair.second = static_cast<KeyType>(type);

		list.push_back(pair);
	}
}

void Events::add(const uint32_t key, const KeyType type) {
	uint32_t x11key = key;
#if defined(__APPLE__) || defined(__CYGWIN__)
	std::map<uint32_t, uint32_t>::iterator it;

	if ((it = sdlkeys.find(key)) != sdlkeys.end()) {
		x11key = it->second;
	}
	else if (type > MOUSE_DOWN) {
		return;
	}
#endif
	list.push_back(std::make_pair(x11key, type));
	number++;
}

void Events::encode(byte *ptr) {
	RETVOID_IF(!ptr);

	number = std::min((int)list.size(), 0xFE);
	*ptr++ = number;

	memcpy(ptr + Consts::u16, &y, Consts::u16);
	memcpy(ptr, &x, Consts::u16);

	RETVOID_IF(!number);
	ptr += Consts::u16 * 2;

	for (auto &p : list) {
		memcpy(ptr, &p.first, Consts::u32);
		ptr[Consts::u32] = p.second;
		ptr += Consts::key;
	}
}

uint16_t &Events::getX(void) {
	return x;
}

uint16_t &Events::getY(void) {
	return y;
}

size_t Events::size(void) const {
	return list.size();
}

std::pair<uint32_t, KeyType> &Events::operator[](const int num) {
	ERR(num >= list.size(), "Non-existent event element requested");
	return list[num];
}

void Events::operator=(const Events &input) {
	x = input.x;
	y = input.y;
	list = input.list;
	number = input.number;
}