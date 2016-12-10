file(GLOB SOURCES "*.c")
add_executable(rival ${SOURCES})

pkg_search_module(LIBUDEV REQUIRED libudev)
pkg_search_module(LIBCONFIG REQUIRED libconfig)
find_package(Threads REQUIRED)

target_link_libraries(
    rival
    ui
    m
    ${LIBCONFIG_LIBRARIES}
    ${LIBUDEV_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(
    rival
    PUBLIC
    ${LIBCONFIG_INCLUDE_DIRS}
    ${LIBUDEV_INCLUDE_DIRS}
)

target_compile_options(
    rival
    PUBLIC
    ${LIBCONFIG_CFLAGS_OTHER}
    ${LIBUDEV_CFLAGS_OTHER}
)
