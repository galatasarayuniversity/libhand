CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
FIND_PACKAGE(OpenCV REQUIRED)

FIND_PACKAGE(LibHand REQUIRED
  PATHS "../hand_cpp/dist"
  NO_MODULE
  NO_DEFAULT_PATH)

include_directories(${LibHand_INCLUDE_DIRS})

#ADD_EXECUTABLE(bend_one_finger bend_one_finger.cc)
#TARGET_LINK_LIBRARIES(bend_one_finger ${LibHand_LIBRARIES})

#ADD_EXECUTABLE(render_hog_descriptor render_hog_descriptor.cc)
#TARGET_LINK_LIBRARIES(render_hog_descriptor ${LibHand_LIBRARIES})

#ADD_EXECUTABLE(file_dialog_test file_dialog_test.cc)
#TARGET_LINK_LIBRARIES(file_dialog_test hand_utils)

ADD_EXECUTABLE(openvibe_hand
                openvibe_hand.cc
                osc/osc/OscTypes.cpp
                osc/osc/OscPrintReceivedElements.cpp
                osc/osc/OscReceivedElements.cpp
                osc/ip/UdpSocket.cpp
                osc/ip/NetworkingUtils.cpp)
TARGET_LINK_LIBRARIES(openvibe_hand ${LibHand_LIBRARIES})
install(TARGETS openvibe_hand DESTINATION ${HAND_LIB_DIST_DIR})

