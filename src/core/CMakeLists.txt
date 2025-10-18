add_library(core STATIC
    performance_monitor.cpp
    logger.cpp
    config_manager.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(core
    Threads::Threads
)