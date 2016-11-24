#include "manager.hpp"
#include "randr.hpp"
#include "util.hpp"

#include <iostream>

namespace wmpp {

namespace {
	std::tuple<xcb_connection_t *, std::size_t> new_connection() {
		int screen_index;
		xcb_connection_t * connection = xcb_connect(nullptr, &screen_index);
		return {connection, std::size_t(screen_index)};
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
	screens_  = getScreens(connection_);
	monitors_ = getRandrMonitors(connection_, screen());
}

manager::~manager() {
	if (disconnect_) {
		xcb_disconnect(connection_);
	}
}

bool manager::poll_one() {
	auto event = wrap_freeing(xcb_poll_for_event(connection_));
	if (!event) return false;
	process_event(*event);
	return true;
}

bool manager::run_one() {
	auto event = wrap_freeing(xcb_wait_for_event(connection_));
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
	(void) event;
	std::cout << "Event received: " << int(event.response_type) << ", " << int(event.full_sequence) << "\n";
}

}
