#pragma once

#include <xcb/xcb.h>

#include <stdexcept>

namespace wmpp {

struct x_error : std::runtime_error {
	xcb_generic_error_t error;

	/// Construct an X error from an xcb_generic_error_t.
	explicit x_error(xcb_generic_error_t error);

	/// Construct an X error from an xcb_generic_error_t and an extra message.
	/**
	 * The passed in message will appear in the what() message.
	 */
	x_error(xcb_generic_error_t error, std::string const & message);
};

}
