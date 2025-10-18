add_library(matching_engine STATIC
    order_book.cpp
    matching_engine.cpp
)

target_include_directories(matching_engine PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(matching_engine
    core
    Threads::Threads
)