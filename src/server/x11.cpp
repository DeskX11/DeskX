
#include "localdesk.h"

x11_server::x11_server(uint8_t val) {
	assert(!inited);

	disp  = XOpenDisplay(nullptr);
	scr   = XDefaultScreen(disp);
	root  = DefaultRootWindow(disp);
	depth = DefaultDepth(disp, scr);
	comp  = val;
	XGetWindowAttributes(disp, root, &attrs);

	img = XShmCreateImage(disp, DefaultVisual(disp, 0), depth, ZPixmap,
						  nullptr, &shm, attrs.width, attrs.height   );
	assert(img);

	sharedmem_alloc();
	XShmAttach(disp, &shm);
	XSync(disp, true);
	XTestGrabControl(disp, true);

	inited = true;
}

x11_server::~x11_server(void) {
	if (!inited) {
		return;
	}

	XShmDetach(disp, &shm);
	XDestroyImage(img);
	shmdt(shm.shmaddr);
	XTestGrabControl(disp, false);
	XCloseDisplay(disp);
}

void x11_server::sharedmem_alloc(void) {
	size_t size = img->bytes_per_line * img->height;
	
	shm.shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0777);
	assert(shm.shmid > 0);

	img->data = (char *)shmat(shm.shmid, 0, 0);
	shm.shmaddr = img->data;
	shm.readOnly = false;
	assert(shm.shmaddr != (char *)-1);

	shmctl(shm.shmid, IPC_RMID, 0);
}

headers x11_server::get_headers(void) {
	headers data;

	data.depth  = depth;
	data.bitmap = img->bitmap_pad;
	data.height = attrs.height;
	data.width  = attrs.width;

	return data;
}

void x11_server::links_table(byte *buff, size_t &size) {
	std::map<uint32_t, size_t>::iterator it1, it2;
	std::vector<pix> list;
	uint32_t tmp = 0;

	pixels_vector(list);

	for (auto &p : list) {
		tmp = p.u32();

		it1 = links.find(tmp);
		if (it1 != links.end()) {
			it1->second++;
			continue;
		}

		links.insert(std::make_pair(tmp, 1));
	}

	while (links.size() > 0xff) {
		it1 = it2 = links.begin();

		for ( ; it1 != links.end(); it1++) {
			if (it1->second < it2->second) {
				it2 = it1;
			}
		}

		links.erase(it2);
	}

	tmp = links.size();
	memcpy(buff, &tmp, U32S);

	for (auto &p : links) {
		memcpy(buff + U32S, &p.first, U32S);
		buff += U32S;
	}

	size = tmp * U32S + U32S;
}

void x11_server::pixels_vector(std::vector<pix> &arr) {
	uint32_t size = img->bytes_per_line * img->height;
	std::map<uint32_t, size_t>::iterator it;
	pix one;

	using std::distance;

	auto cmp = [&](char *pixs, pix &d) {
		return abs(pixs[0] - d.r)
			 + abs(pixs[1] - d.g)
			 + abs(pixs[2] - d.b) < comp;
	};

	auto cpy = [](char *pixs, pix &one) {
		one.r = pixs[0];
		one.g = pixs[1];
		one.b = pixs[2];
	};

	XShmGetImage(disp, root, img, 0, 0, AllPlanes);
	cpy(img->data, one);

	for (uint32_t i = 0; i < size; i += 4) {
		if (cmp(img->data + i, one) && one.num < 0xff) {
			one.num++;
			continue;
		}

		it = links.find(one.u32());

		if (it != links.end()) {
			one.link_id = distance(links.begin(), it);
			one.link = true;
		}

		arr.push_back(one);
		cpy(img->data + i, one);
		one.num = 1;
		one.link = false;
	}
}

void x11_server::set_mouse(uint16_t x, uint16_t y) {
	XTestFakeMotionEvent(disp, 0, x, y, 0);
}

void x11_server::set_events(byte *buff, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		bool f = true;
		uint16_t shift = i * 2;

		switch (buff[i * 2]) {
		// mouse events
		case 0:
			f = false;
		case 1:
			XTestFakeButtonEvent(disp, buff[shift + 1], f, 0);
			continue;
		// keyboard events
		case 2:
			f = false;
		case 3:
			XTestFakeKeyEvent(disp, buff[shift + 1], f, 0);
		default:
			continue;
		}	
	}
}

int x11_server::pixs_num(void) {
	return img->width * img->height;
}
