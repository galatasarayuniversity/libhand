CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)


PROJECT(LibHand)

SET(HAND_LIB_DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/hand_cpp/modules)

add_subdirectory(hand_cpp)
add_subdirectory(examples)
