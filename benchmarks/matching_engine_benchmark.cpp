#include <benchmark/benchmark.h>
#include "trading_engine/matching_engine/order_book.hpp"
#include "trading_engine/types.hpp"

using namespace trading_engine;

static void BM_OrderBookAddOrder(benchmark::State& state) {
    matching_engine::OrderBook order_book("AAPL");
    OrderId order_id = 1;
    
    for (auto _ : state) {
        Order order{
            .id = order_id++,
            .symbol = "AAPL",
            .side = Side::BUY,
            .type = OrderType::LIMIT,
            .price = 15000 + (order_id % 100), // Vary price slightly
            .quantity = 100,
            .timestamp = std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
        };
        
        benchmark::DoNotOptimize(order_book.add_order(order));
    }
    
    state.SetItemsProcessed(state.iterations());
}

static void BM_OrderBookMatching(benchmark::State& state) {
    matching_engine::OrderBook order_book("AAPL");
    
    // Pre-populate with buy orders
    for (int i = 0; i < 1000; ++i) {
        Order buy_order{
            .id = static_cast<OrderId>(i),
            .symbol = "AAPL",
            .side = Side::BUY,
            .type = OrderType::LIMIT,
            .price = 14900 + (i % 100),
            .quantity = 100,
            .timestamp = std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
        };
        order_book.add_order(buy_order);
    }
    
    OrderId sell_order_id = 10000;
    for (auto _ : state) {
        Order sell_order{
            .id = sell_order_id++,
            .symbol = "AAPL",
            .side = Side::SELL,
            .type = OrderType::LIMIT,
            .price = 14950,
            .quantity = 100,
            .timestamp = std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
        };
        
        benchmark::DoNotOptimize(order_book.add_order(sell_order));
    }
    
    state.SetItemsProcessed(state.iterations());
}

static void BM_VWAPCalculation(benchmark::State& state) {
    matching_engine::OrderBook order_book("AAPL");
    
    // Pre-populate with orders
    for (int i = 0; i < state.range(0); ++i) {
        Order order{
            .id = static_cast<OrderId>(i),
            .symbol = "AAPL",
            .side = (i % 2 == 0) ? Side::BUY : Side::SELL,
            .type = OrderType::LIMIT,
            .price = 15000 + (i % 200) - 100,
            .quantity = 100 + (i % 50),
            .timestamp = std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
        };
        order_book.add_order(order);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(order_book.calculate_vwap(Side::BUY, 10000));
    }
    
    state.SetComplexityN(state.range(0));
    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_OrderBookAddOrder)
    ->Unit(benchmark::kNanosecond)
    ->Iterations(1000000);

BENCHMARK(BM_OrderBookMatching)
    ->Unit(benchmark::kNanosecond)
    ->Iterations(100000);

BENCHMARK(BM_VWAPCalculation)
    ->RangeMultiplier(2)
    ->Range(100, 10000)
    ->Unit(benchmark::kMicrosecond)
    ->Complexity();

BENCHMARK_MAIN();