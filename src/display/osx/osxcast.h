
#ifndef DESKX_DISPLAY_OSX_BINDING_H
#define DESKX_DISPLAY_OSX_BINDING_H

#ifndef __OBJC__
extern "C" {
#endif

void *
osxcast_init(const char *);

void
osxcast_close(void *);

#ifndef __OBJC__
}
#endif
#endif