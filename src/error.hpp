#pragma once
#include "util.hpp"

#include <xcb/xcb.h>

#include <cstdlib>
#include <stdexcept>

namespace wmpp {

/// An exception class for XCB errors.
/**
 * The exception object takes ownership of the given error pointer and will free it
 * when the object is destroyed.
 */
struct x_error : std::runtime_error {
	free_pointer<xcb_generic_error_t> error;

	/// Construct an X error from an xcb_generic_error_t.
	explicit x_error(free_pointer<xcb_generic_error_t> error);

	/// Construct an X error from an xcb_generic_error_t.
	explicit x_error(xcb_generic_error_t * && error);

	/// Construct an X error from an xcb_generic_error_t and an extra message.
	/**
	 * The passed in message will appear in the what() message.
	 */
	x_error(free_pointer<xcb_generic_error_t> error, std::string const & message);

	/// Construct an X error from an xcb_generic_error_t and an extra message.
	/**
	 * The passed in message will appear in the what() message.
	 */
	x_error(xcb_generic_error_t * && error, std::string const & message);
};

inline void check_or_throw(xcb_connection_t * connection, xcb_void_cookie_t cookie) {
	auto error = wrap_freeing(xcb_request_check(connection, cookie));
	if (error) throw x_error(std::move(error));
}

}
