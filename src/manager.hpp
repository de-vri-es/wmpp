#include <xcb/xcb.h>
#include <xcb/randr.h>

#include <functional>
#include <tuple>
#include <vector>

namespace wmpp {

/// Get a list of screens from a screen iterator.
std::vector<xcb_screen_t> getScreens(xcb_screen_iterator_t screens);

/// Get a list of screens from setup information.
std::vector<xcb_screen_t> getScreens(xcb_setup_t const * setup);

/// Get a list of screens from a connection.
std::vector<xcb_screen_t> getScreens(xcb_connection_t * connection);

class manager {
public:
	std::function<void (manager & manager, xcb_map_request_event_t       & event)> on_map_request;
	std::function<void (manager & manager, xcb_configure_request_event_t & event)> on_configure_request;
	std::function<void (manager & manager, xcb_circulate_request_event_t & event)> on_circulate_request;
	std::function<void (manager & manager, xcb_generic_event_t           & event)> on_other_event;

protected:
	/// Connection with X server.
	xcb_connection_t * connection_;

	/// List of X screens.
	std::vector<xcb_screen_t> screens_;

	/// Index of the X screen we're managing.
	std::size_t screen_index_;

	/// List of monitor information.
	std::vector<xcb_randr_monitor_info_t> monitors_;

	/// If true, disconnect in the destructor.
	bool disconnect_ = false;

	/// Flag to stop a running event loop.
	bool stop_ = false;

public:
	/// Create a new window magager on the default display using the default screen.
	manager();

	/// Create a new window magager using the given connection and screen index.
	manager(
		xcb_connection_t * connection,  ///< The xcb connection.
		std::size_t screen_index,       ///< The screen index to use.
		bool auto_disconnect = false    ///< If true, close the connection when the manager is destroyed.
	);

	/// Destroy the window manager.
	/**
	 * If the manager was default constructed or constructed with auto_disconnect set to true,
	 * the held xcb connection will be closed.
	 */
	~manager();

protected:
	/// Internal constructor which all public constructors delegate to.
	manager(std::tuple<xcb_connection_t *, std::size_t> connection_info, bool auto_disconnect);

public:
	/// Get the XCB connection used by the manager.
	xcb_connection_t       * connection()       { return connection_; }
	xcb_connection_t const * connection() const { return connection_; }

	/// Get the screen index of the managed screen.
	/**
	 * This refers to the X11 screen abstraction, not RandR monitors.
	 */
	std::size_t screen_index() const { return screen_index_; }

	/// Get the managed screen.
	/**
	 * This refers to the X11 screen abstraction, not RandR monitors.
	 */
	xcb_screen_t       & screen()       { return screens_[screen_index_]; }
	xcb_screen_t const & screen() const { return screens_[screen_index_]; }

	/// Get the root window of the managed screen.
	xcb_window_t root_window() const { return screen().root; }

	/// Get a list of monitors.
	/**
	 * This refers RandR monitor concept.
	 */
	std::vector<xcb_randr_monitor_info_t> const & monitors() const { return monitors_; }

	/// Handle a single event. If no events are available, return immedeately.
	/**
	 * \return True if an event was handled, false if no event was available or an error occured.
	 */
	bool poll_one();

	/// Handle a single event. If no events are available, block until an event becomes available.
	/**
	 * \return True if an event was handled, false if an error occured.
	 */
	bool run_one();

	/// Keep running the event loop.
	void run();

	/// Stop a previous invocation of run() after it finished the current event.
	void stop();

protected:
	/// Process a single event.
	void process_event(xcb_generic_event_t & event);
};

}
