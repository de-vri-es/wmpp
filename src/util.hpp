#pragma once
#include <cstdlib>
#include <memory>

namespace wmpp {

struct free_deleter {
	template<typename T>
	void operator() (T * pointer) {
		std::free(pointer);
	}
};

template<typename T>
using free_pointer = std::unique_ptr<T, free_deleter>;

template<typename T>
free_pointer<T> wrap_freeing(T * pointer) {
	return free_pointer<T>{pointer};
}

}
