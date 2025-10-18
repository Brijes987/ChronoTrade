add_library(market_data STATIC
    feed_handler.cpp
)

target_include_directories(market_data PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(market_data
    core
    networking
    Threads::Threads
)