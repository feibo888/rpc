# file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cc ${CMAKE_CURRENT_SOURCE_DIR}/../*.pb.cc)

# set(SRC_LIST userservice.cc ../user.pb.cc)
set(SRC_LIST friendserver.cc ../friend.pb.cc)
add_executable(provider ${SRC_LIST})
target_link_libraries(provider mprpc protobuf)