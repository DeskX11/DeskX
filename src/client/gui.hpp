
#ifndef DESKX_CLIENT_GUI_HPP
#define DESKX_CLIENT_GUI_HPP

#include <display.hpp>

namespace client {
namespace gui {

bool
init(void);

const size_t &
width(void);

const size_t &
height(void);

byte *
window(const size_t &, const size_t &);

void
close(void);

bool
refresh(void);

std::pair<bool, bool>
events(display::events &);

}
}

#endif