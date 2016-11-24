#include "error.hpp"

#include <string>

namespace wmpp {

namespace {
	static std::string formatMessage(xcb_generic_error_t error) {
		return "X protocol error: " + std::to_string(error.error_code)
			+ ", response type: " + std::to_string(error.response_type)
			+ ", sequence: " + std::to_string(error.sequence)
			+ ", resource id: " + std::to_string(error.resource_id)
			+ ", request code: " + std::to_string(error.major_code) + ":" + std::to_string(error.minor_code)
			+ ", full sequence: " + std::to_string(error.full_sequence)
		;
	}

	static std::string formatMessage(xcb_generic_error_t error, std::string const & message) {
		return formatMessage(error) + ". " + message;
	}
}

XError::XError(xcb_generic_error_t error) :
	std::runtime_error(formatMessage(error)), error(error) {}

XError::XError(xcb_generic_error_t error, std::string const & message) :
	std::runtime_error(formatMessage(error, message)), error(error) {}

}