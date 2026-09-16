cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB_RECURSE rnoh_touch_through_view_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_touch_through_view SHARED ${rnoh_touch_through_view_SRC})
target_include_directories(rnoh_touch_through_view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(rnoh_touch_through_view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/generated)
target_link_libraries(rnoh_touch_through_view PUBLIC rnoh)
target_link_libraries(rnoh_touch_through_view PUBLIC libace_ndk.z.so)
