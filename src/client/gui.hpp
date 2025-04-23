
#ifndef DESKX_CLIENT_GUI_HPP
#define DESKX_CLIENT_GUI_HPP

#include <display.hpp>

namespace client {
namespace gui {

byte *
init(const size_t &, const size_t &);

void
close(void);

bool
refresh(void);

std::pair<bool, bool>
events(display::events &);

}
}

#endif