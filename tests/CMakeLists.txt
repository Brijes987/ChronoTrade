add_executable(unit_tests
    test_order_book.cpp
    test_lockfree_queue.cpp
    test_pool_allocator.cpp
    test_risk_manager.cpp
    test_market_data.cpp
)

target_link_libraries(unit_tests
    matching_engine
    market_data
    risk_management
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

add_test(NAME UnitTests COMMAND unit_tests)

# Integration tests
add_executable(integration_tests
    integration_test_trading_engine.cpp
)

target_link_libraries(integration_tests
    matching_engine
    market_data
    risk_management
    networking
    grpc_api
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

add_test(NAME IntegrationTests COMMAND integration_tests)