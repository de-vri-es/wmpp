#include "manager.hpp"

#include <vector>
#include <iostream>

int main() {
	wmpp::manager manager{};

	std::cout << "Running on screen " << manager.screen_index() << ".\n";
	std::cout << "Screen mode: " << manager.screen().width_in_pixels << "x" << manager.screen().height_in_pixels << "x" << int(manager.screen().root_depth) << "\n";

	for (std::size_t i = 0; i < manager.monitors().size(); ++i) {
		auto & monitor = manager.monitors()[i];
		std::cout << "Monitor " << i << ": " << monitor.width << "x" << monitor.height << "+" << monitor.x << "," << monitor.y << "\n";
	}

	manager.run();

}
