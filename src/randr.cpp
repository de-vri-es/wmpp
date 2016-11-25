#include "randr.hpp"
#include "util.hpp"

#include <cstdlib>

namespace wmpp {

std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_randr_monitor_info_iterator_t iterator) {
	std::vector<xcb_randr_monitor_info_t> result;
	result.reserve(iterator.rem);
	for (; iterator.rem > 0; xcb_randr_monitor_info_next(&iterator)) {
		result.push_back(*iterator.data);
	}
	return result;
}

std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_connection_t * xcb, xcb_window_t window) {
	xcb_generic_error_t * error = nullptr;
	auto cookie = xcb_randr_get_monitors(xcb, window, true);
	auto reply  = wrap_freeing(xcb_randr_get_monitors_reply(xcb, cookie, &error));
	if (error) throw x_error(std::move(error));
	return getRandrMonitors(xcb_randr_get_monitors_monitors_iterator(reply.get()));
}

std::vector<xcb_randr_monitor_info_t> getRandrMonitors(xcb_connection_t * xcb, xcb_screen_t const & screen) {
	return getRandrMonitors(xcb, screen.root);
}

}
