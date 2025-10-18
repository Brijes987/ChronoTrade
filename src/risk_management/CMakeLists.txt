add_library(risk_management STATIC
    risk_manager.cpp
)

target_include_directories(risk_management PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(risk_management
    core
    Threads::Threads
)