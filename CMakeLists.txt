cmake_minimum_required(VERSION 3.12)

project(hello-grpc
    LANGUAGES CXX
    VERSION 1.0.0
)

# Options for using pre-generated proto files
option(USE_GENERATED_PROTO "Use pre-generated proto files" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Protobuf and gRPC compiler
find_program(PROTOC protoc)
if(NOT PROTOC)
  message(FATAL_ERROR "protoc not found")
endif()
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
if(NOT GRPC_CPP_PLUGIN)
  message(FATAL_ERROR "grpc_cpp_plugin not found")
endif()

# Find gRPC and Protobuf dependencies
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Proto files to be compiled
set(PROTO_FILES
  hello.proto
)

set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/protos)

if(USE_GENERATED_PROTO)
  set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)
  set(PROTO_SRCS
    ${PROTO_SRC_DIR}/hello.pb.cc
    ${PROTO_SRC_DIR}/hello.grpc.pb.cc
  )
  set(PROTO_HDRS
    ${PROTO_SRC_DIR}/hello.pb.h
    ${PROTO_SRC_DIR}/hello.grpc.pb.h
  )
else()
  set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
  file(MAKE_DIRECTORY ${PROTO_SRC_DIR})

  set(PROTO_SRCS)
  set(PROTO_HDRS)
  foreach(PROTO_FILE ${PROTO_FILES})
    get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
    set(PROTO_SRC ${PROTO_SRC_DIR}/${PROTO_NAME}.pb.cc)
    set(PROTO_HDR ${PROTO_SRC_DIR}/${PROTO_NAME}.pb.h)
    set(GRPC_SRC ${PROTO_SRC_DIR}/${PROTO_NAME}.grpc.pb.cc)
    set(GRPC_HDR ${PROTO_SRC_DIR}/${PROTO_NAME}.grpc.pb.h)
    add_custom_command(
      OUTPUT ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}
      COMMAND ${PROTOC}
      ARGS --proto_path=${PROTO_PATH} --cpp_out ${PROTO_SRC_DIR} --grpc_out ${PROTO_SRC_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PROTO_FILE}
      DEPENDS ${PROTO_PATH}/${PROTO_FILE}
      COMMENT "Generating ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}"
      VERBATIM
    )
    list(APPEND PROTO_SRCS ${PROTO_SRC} ${GRPC_SRC})
    list(APPEND PROTO_HDRS ${PROTO_HDR} ${GRPC_HDR})
  endforeach()
endif()

# Compile proto files
add_library(proto ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto PUBLIC ${PROTO_SRC_DIR})
target_link_libraries(proto PUBLIC gRPC::grpc++_unsecure)

# Compile server and client
add_executable(server src/server.cpp)
target_link_libraries(server proto gRPC::grpc++_unsecure)

add_executable(client src/client.cpp)
target_link_libraries(client proto gRPC::grpc++_unsecure)
