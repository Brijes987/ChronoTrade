add_library(grpc_api STATIC
    trading_service.cpp
)

target_include_directories(grpc_api PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(grpc_api
    core
    gRPC::grpc++
    protobuf::libprotobuf
    Threads::Threads
)