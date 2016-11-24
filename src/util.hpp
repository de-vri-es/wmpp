#pragma once
#include <cstdlib>
#include <memory>

namespace wmpp {

/// Deleter that calls std::free.
struct free_deleter {
	template<typename T>
	void operator() (T * pointer) {
		std::free(pointer);
	}
};

/// A unique_ptr using free_deleter.
template<typename T>
using free_pointer = std::unique_ptr<T, free_deleter>;

/// Wrap a raw pointer in a free_pointer.
template<typename T>
free_pointer<T> wrap_freeing(T * pointer) {
	return free_pointer<T>{pointer};
}

}
