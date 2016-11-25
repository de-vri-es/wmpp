#include "manager.hpp"
#include "randr.hpp"
#include "util.hpp"
#include "error.hpp"

#include <xcb/xcb_event.h>

#include <iostream>

namespace wmpp {

namespace {
	std::tuple<xcb_connection_t *, std::size_t> new_connection() {
		int screen_index;
		xcb_connection_t * connection = xcb_connect(nullptr, &screen_index);
		return {connection, std::size_t(screen_index)};
	}

	void default_on_map_request(manager & manager, xcb_map_request_event_t & event) {
		auto cookie = xcb_map_window_checked(manager.connection(), event.window);
		check_or_throw(manager.connection(), cookie);
	}
}

std::vector<xcb_screen_t> getScreens(xcb_screen_iterator_t screens) {
	std::vector<xcb_screen_t> result;
	result.reserve(screens.rem);
	while (screens.rem > 0) {
		result.push_back(*screens.data);
		xcb_screen_next(&screens);
	}

	return result;
}

std::vector<xcb_screen_t> getScreens(xcb_setup_t const * setup) {
	return getScreens(xcb_setup_roots_iterator(setup));
}

std::vector<xcb_screen_t> getScreens(xcb_connection_t * connection) {
	return getScreens(xcb_setup_roots_iterator(xcb_get_setup(connection)));
}

manager::manager() : manager(new_connection(), true) {}

manager::manager(xcb_connection_t * connection, std::size_t screen_index, bool auto_disconnect) :
	manager(std::make_tuple(connection, screen_index), auto_disconnect) {}

manager::manager(std::tuple<xcb_connection_t *, std::size_t> connection_info, bool auto_disconnect) :
	connection_{std::get<0>(connection_info)},
	screen_index_{std::get<1>(connection_info)},
	disconnect_{auto_disconnect}
{
	on_map_request = &default_on_map_request;
	screens_       = getScreens(connection());
	monitors_      = getRandrMonitors(connection(), screen());

	std::uint32_t events = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
	auto cookie = xcb_change_window_attributes_checked(connection(), root_window(), XCB_CW_EVENT_MASK, &events);
	check_or_throw(connection(), cookie);
}

manager::~manager() {
	if (disconnect_) {
		xcb_disconnect(connection());
	}
}

bool manager::poll_one() {
	auto event = wrap_freeing(xcb_poll_for_event(connection()));
	if (!event) return false;
	process_event(*event);
	return true;
}

bool manager::run_one() {
	auto event = wrap_freeing(xcb_wait_for_event(connection()));
	if (!event) return false;
	process_event(*event);
	return true;
}

void manager::run() {
	stop_ = false;
	while (!stop_) {
		run_one();
	}
}

void manager::stop() {
	stop_ = true;
}

void manager::process_event(xcb_generic_event_t & event) {
	int type  = event.response_type & XCB_EVENT_RESPONSE_TYPE_MASK;
	bool sent = event.response_type & ~XCB_EVENT_RESPONSE_TYPE_MASK;
	(void) sent;
	std::cout << "Event received: " << int(type) << ": " << xcb_event_get_label(type) << ", " << int(event.full_sequence) << "\n";
	switch (type) {
		case XCB_MAP_REQUEST:       if (on_map_request)       on_map_request(*this, reinterpret_cast<xcb_map_request_event_t &>(event));
		case XCB_CONFIGURE_REQUEST: if (on_configure_request) on_configure_request(*this, reinterpret_cast<xcb_configure_request_event_t &>(event));
		case XCB_CIRCULATE_REQUEST: if (on_circulate_request) on_circulate_request(*this, reinterpret_cast<xcb_circulate_request_event_t &>(event));
		default: if (on_other_event) on_other_event(*this, event);
	}
}

}
