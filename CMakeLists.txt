project(wmpp)
cmake_minimum_required(VERSION 3.3)

add_library(${PROJECT_NAME} SHARED
	src/error.cpp
	src/manager.cpp
	src/randr.cpp
)

find_package(PkgConfig)
pkg_check_modules(XCB xcb xcb-randr)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(${PROJECT_NAME} PUBLIC ${XCB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${XCB_LIBRARIES})

add_executable(example src/main.cpp)
target_link_libraries(example ${PROJECT_NAME})
