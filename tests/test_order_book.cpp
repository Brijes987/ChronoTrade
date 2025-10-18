#include <gtest/gtest.h>
#include "trading_engine/matching_engine/order_book.hpp"
#include "trading_engine/types.hpp"

using namespace trading_engine;

class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        order_book = std::make_unique<matching_engine::OrderBook>("AAPL");
    }
    
    std::unique_ptr<matching_engine::OrderBook> order_book;
};

TEST_F(OrderBookTest, AddBuyOrder) {
    Order order{
        .id = 1,
        .symbol = "AAPL",
        .side = Side::BUY,
        .type = OrderType::LIMIT,
        .price = 15000, // $150.00
        .quantity = 100,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    
    OrderId result = order_book->add_order(order);
    EXPECT_EQ(result, 1);
}

TEST_F(OrderBookTest, AddSellOrder) {
    Order order{
        .id = 2,
        .symbol = "AAPL",
        .side = Side::SELL,
        .type = OrderType::LIMIT,
        .price = 15100, // $151.00
        .quantity = 200,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    
    OrderId result = order_book->add_order(order);
    EXPECT_EQ(result, 2);
}

TEST_F(OrderBookTest, OrderMatching) {
    // Add buy order
    Order buy_order{
        .id = 1,
        .symbol = "AAPL",
        .side = Side::BUY,
        .type = OrderType::LIMIT,
        .price = 15000,
        .quantity = 100,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    order_book->add_order(buy_order);
    
    // Add matching sell order
    Order sell_order{
        .id = 2,
        .symbol = "AAPL",
        .side = Side::SELL,
        .type = OrderType::LIMIT,
        .price = 15000, // Same price - should match
        .quantity = 50,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    
    bool trade_executed = false;
    order_book->set_trade_callback([&trade_executed](const Trade& trade) {
        trade_executed = true;
        EXPECT_EQ(trade.price, 15000);
        EXPECT_EQ(trade.quantity, 50);
    });
    
    order_book->add_order(sell_order);
    EXPECT_TRUE(trade_executed);
}

TEST_F(OrderBookTest, MarketData) {
    // Add some orders to create market data
    Order buy_order{
        .id = 1,
        .symbol = "AAPL",
        .side = Side::BUY,
        .type = OrderType::LIMIT,
        .price = 14950,
        .quantity = 100,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    order_book->add_order(buy_order);
    
    Order sell_order{
        .id = 2,
        .symbol = "AAPL",
        .side = Side::SELL,
        .type = OrderType::LIMIT,
        .price = 15050,
        .quantity = 200,
        .timestamp = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
    };
    order_book->add_order(sell_order);
    
    MarketData market_data = order_book->get_market_data();
    EXPECT_EQ(market_data.symbol, "AAPL");
    EXPECT_EQ(market_data.bid_price, 14950);
    EXPECT_EQ(market_data.ask_price, 15050);
    EXPECT_EQ(market_data.bid_size, 100);
    EXPECT_EQ(market_data.ask_size, 200);
}