# gRPC Lib this is where the protobuf and grpc magic happens.

add_library(GrpcLib
  STATIC
    Tweet.proto
)

target_include_directories(GrpcLib
  PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET GrpcLib LANGUAGE cpp)
protobuf_generate(
  TARGET
    GrpcLib
  LANGUAGE
    grpc
  GENERATE_EXTENSIONS
    .grpc.pb.h
    .grpc.pb.cc
  PLUGIN
    "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)

target_link_libraries(GrpcLib
  PUBLIC
    protobuf::libprotoc
    protobuf::libprotobuf 
    protobuf::libprotobuf-lite
    gRPC::gpr 
    gRPC::grpc 
    gRPC::grpc++
)

set_property(TARGET GrpcLib PROPERTY FOLDER "Library")
