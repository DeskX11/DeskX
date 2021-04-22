
#include "localdesk.h"

x11_server::x11_server(uint8_t val) {
	assert(!inited);

	disp  = XOpenDisplay(nullptr);
	scr   = XDefaultScreen(disp);
	root  = DefaultRootWindow(disp);
	depth = DefaultDepth(disp, scr);
	comp = 12;
	XGetWindowAttributes(disp, root, &attrs);

	img = XShmCreateImage(disp, DefaultVisual(disp, 0), depth, ZPixmap,
						  nullptr, &shm, attrs.width, attrs.height);
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

char *x11_server::pix_pointer(void) {
	XSync(disp, true);
	XShmGetImage(disp, root, img, 0, 0, AllPlanes);
	return img->data;
}

void x11_server::pixels_vector(std::vector<pix> &arr) {
	constexpr size_t lim = std::numeric_limits<uint16_t>::max() - 1;
	uint32_t size = img->bytes_per_line * img->height;
	pix one;

	auto cmp = [&](char *pixs, pix &d) {
		return abs(pixs[0] - d.r) < 10
			&& abs(pixs[1] - d.g) < 10
			&& abs(pixs[2] - d.b) < 10;
	};

	auto cpy = [](char *pixs, pix &one) {
		one.r = pixs[0];
		one.g = pixs[1];
		one.b = pixs[2];
	};

	char *from = pix_pointer();
	cpy(from, one);

	for (uint32_t i = 0; i < size; i += 4) {
		if (cmp(from + i, one) && one.num < lim) {
			one.num++;
			continue;
		}

		arr.push_back(one);
		cpy(from + i, one);
		one.num = 1;
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
