#pragma once

#include <xcb/xcb.h>

#include <stdexcept>

namespace wmpp {

struct XError : std::runtime_error {
	xcb_generic_error_t error;

	XError(xcb_generic_error_t error);
	XError(xcb_generic_error_t error, std::string const & message);
};

}
