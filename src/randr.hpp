#pragma once

#include "error.hpp"

#include <xcb/randr.h>

#include <vector>

namespace wmpp {

/// Get a list of monitor info from a monitor info iterator.
std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_randr_monitor_info_iterator_t iterator);

/// Get a list of monitor info for the screen of a window.
std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_connection_t * xcb, xcb_window_t window);

/// Get a list of monitor info for a screen.
std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_connection_t * xcb, xcb_screen_t const & screen);

}
