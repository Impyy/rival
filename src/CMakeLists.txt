file(GLOB SOURCES "*.c")
add_executable(rival ${SOURCES})

pkg_search_module(LIBUDEV REQUIRED libudev)
find_package(Threads REQUIRED)

target_link_libraries(rival ui m ${LIBUDEV_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(rival PUBLIC ${LIBUDEV_INCLUDE_DIRS})
target_compile_options(rival PUBLIC ${LIBUDEV_CFLAGS_OTHER})
