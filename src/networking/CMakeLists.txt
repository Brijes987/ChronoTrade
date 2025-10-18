add_library(networking STATIC
    udp_receiver.cpp
)

target_include_directories(networking PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(networking
    core
    Threads::Threads
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(networking ws2_32)
endif()