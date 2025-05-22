
#ifndef DESKX_DISPLAY_OSX_BINDING_H
#define DESKX_DISPLAY_OSX_BINDING_H

#ifndef __OBJC__
extern "C" {
#else
#define DIE(cond) if ((cond)) exit(1)
#endif

void *
osxcast_init(int *);

void
osxcast_close(void *);

#ifndef __OBJC__
}
#endif
#endif