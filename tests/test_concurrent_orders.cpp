#include <gtest/gtest.h>
#include "trading_engine/matching_engine/order_book.hpp"
#include "trading_engine/types.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace trading_engine;
using namespace trading_engine::matching_engine;

class ConcurrentOrderTest : public ::testing::Test {
protected:
    void SetUp() override {
        order_book = std::make_unique<OrderBook>("AAPL");
        trade_count.store(0);
        error_count.store(0);
    }

    void TearDown() override {
        order_book.reset();
    }

    std::unique_ptr<OrderBook> order_book;
    std::atomic<uint64_t> trade_count{0};
    std::atomic<uint64_t> error_count{0};
};

// Stress test: 10K concurrent goroutines (threads) inserting orders
TEST_F(ConcurrentOrderTest, StressTest10KConcurrentInsertions) {
    constexpr size_t num_threads = 10000;
    constexpr size_t orders_per_thread = 10;
    
    order_book->set_trade_callback([this](const Trade& trade) {
        trade_count.fetch_add(1, std::memory_order_relaxed);
    });
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Launch 10K threads
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, i, orders_per_thread]() {
            for (size_t j = 0; j < orders_per_thread; ++j) {
                try {
                    Order order;
                    order.symbol = "AAPL";
                    order.order_id = i * orders_per_thread + j;
                    order.client_id = "client_" + std::to_string(i);
                    order.side = (i % 2 == 0) ? Side::Buy : Side::Sell;
                    order.type = OrderType::Limit;
                    order.price = 15000 + (i % 100); // Price range: 15000-15099
                    order.quantity = 100;
                    order.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
                    
                    order_book->add_order(order);
                } catch (const std::exception& e) {
                    error_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time).count();
    
    // Verify results
    uint64_t total_orders = num_threads * orders_per_thread;
    uint64_t errors = error_count.load();
    uint64_t trades = trade_count.load();
    
    std::cout << "\n=== Concurrent Order Insertion Stress Test ===" << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Orders per thread: " << orders_per_thread << std::endl;
    std::cout << "Total orders: " << total_orders << std::endl;
    std::cout << "Duration: " << duration << " ms" << std::endl;
    std::cout << "Throughput: " << (total_orders * 1000 / duration) << " orders/sec" << std::endl;
    std::cout << "Trades executed: " << trades << std::endl;
    std::cout << "Errors: " << errors << std::endl;
    
    // Assertions
    EXPECT_LT(errors, total_orders * 0.01) << "Error rate should be less than 1%";
    EXPECT_GT(trades, 0) << "Should have executed some trades";
}

// Test concurrent buy and sell orders
TEST_F(ConcurrentOrderTest, ConcurrentBuyAndSellOrders) {
    constexpr size_t num_threads = 1000;
    constexpr Price base_price = 15000;
    
    std::atomic<uint64_t> buy_count{0};
    std::atomic<uint64_t> sell_count{0};
    
    order_book->set_trade_callback([this](const Trade& trade) {
        trade_count.fetch_add(1, std::memory_order_relaxed);
    });
    
    std::vector<std::thread> threads;
    
    // Launch buy threads
    for (size_t i = 0; i < num_threads / 2; ++i) {
        threads.emplace_back([this, i, base_price, &buy_count]() {
            Order order;
            order.symbol = "AAPL";
            order.order_id = i;
            order.client_id = "buyer_" + std::to_string(i);
            order.side = Side::Buy;
            order.type = OrderType::Limit;
            order.price = base_price + (i % 10);
            order.quantity = 100;
            order.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
            
            order_book->add_order(order);
            buy_count.fetch_add(1, std::memory_order_relaxed);
        });
    }
    
    // Launch sell threads
    for (size_t i = 0; i < num_threads / 2; ++i) {
        threads.emplace_back([this, i, base_price, &sell_count]() {
            Order order;
            order.symbol = "AAPL";
            order.order_id = i + num_threads / 2;
            order.client_id = "seller_" + std::to_string(i);
            order.side = Side::Sell;
            order.type = OrderType::Limit;
            order.price = base_price + (i % 10);
            order.quantity = 100;
            order.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
            
            order_book->add_order(order);
            sell_count.fetch_add(1, std::memory_order_relaxed);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(buy_count.load(), num_threads / 2);
    EXPECT_EQ(sell_count.load(), num_threads / 2);
    EXPECT_GT(trade_count.load(), 0) << "Should have matched some orders";
}

// Test order book consistency under concurrent modifications
TEST_F(ConcurrentOrderTest, OrderBookConsistencyUnderLoad) {
    constexpr size_t num_threads = 500;
    constexpr size_t operations_per_thread = 100;
    
    std::vector<std::thread> threads;
    std::atomic<uint64_t> successful_ops{0};
    
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([this, i, operations_per_thread, &successful_ops]() {
            for (size_t j = 0; j < operations_per_thread; ++j) {
                Order order;
                order.symbol = "AAPL";
                order.order_id = i * operations_per_thread + j;
                order.client_id = "client_" + std::to_string(i);
                order.side = (j % 2 == 0) ? Side::Buy : Side::Sell;
                order.type = OrderType::Limit;
                order.price = 15000 + (j % 50);
                order.quantity = 100;
                order.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
                
                try {
                    order_book->add_order(order);
                    successful_ops.fetch_add(1, std::memory_order_relaxed);
                    
                    // Occasionally cancel orders
                    if (j % 10 == 0 && j > 0) {
                        order_book->cancel_order(i * operations_per_thread + j - 1);
                    }
                } catch (...) {
                    // Ignore errors for this test
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    uint64_t total_ops = num_threads * operations_per_thread;
    uint64_t success = successful_ops.load();
    
    std::cout << "\n=== Order Book Consistency Test ===" << std::endl;
    std::cout << "Total operations: " << total_ops << std::endl;
    std::cout << "Successful operations: " << success << std::endl;
    std::cout << "Success rate: " << (success * 100.0 / total_ops) << "%" << std::endl;
    
    EXPECT_GT(success, total_ops * 0.95) << "Success rate should be > 95%";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
