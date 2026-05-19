#pragma once

#include "trading_engine/types.hpp"
#include "trading_engine/memory/pool_allocator.hpp"
#include <map>
#include <vector>
#include <functional>
#include <immintrin.h> // For SIMD

namespace trading_engine::matching_engine {

class OrderBook {
private:
    struct PriceLevel {
        Price price;
        Quantity total_quantity{0};
        std::vector<Order*> orders;
        
        void add_order(Order* order) {
            orders.push_back(order);
            total_quantity += order->remaining_quantity();
        }
        
        void remove_order(Order* order) {
            auto it = std::find(orders.begin(), orders.end(), order);
            if (it != orders.end()) {
                total_quantity -= order->remaining_quantity();
                orders.erase(it);
            }
        }
    };
    
    // Custom comparator for price-time priority with reduced branch misprediction
    struct BuyComparator {
        bool operator()(const Price& a, const Price& b) const noexcept {
            // Branchless comparison: higher price wins
            return a > b;
        }
    };
    
    struct SellComparator {
        bool operator()(const Price& a, const Price& b) const noexcept {
            // Branchless comparison: lower price wins
            return a < b;
        }
    };
    
    using BuyLevels = std::map<Price, PriceLevel, BuyComparator>;
    using SellLevels = std::map<Price, PriceLevel, SellComparator>;
    
    Symbol symbol_;
    BuyLevels buy_levels_;
    SellLevels sell_levels_;
    memory::PoolAllocator<Order> order_allocator_;
    
    std::function<void(const Trade&)> trade_callback_;
    std::function<void(const Order&)> order_update_callback_;

public:
    explicit OrderBook(const Symbol& symbol) : symbol_(symbol) {}
    
    void set_trade_callback(std::function<void(const Trade&)> callback) {
        trade_callback_ = std::move(callback);
    }
    
    void set_order_update_callback(std::function<void(const Order&)> callback) {
        order_update_callback_ = std::move(callback);
    }
    
    OrderId add_order(const Order& order);
    bool cancel_order(OrderId order_id);
    bool modify_order(OrderId order_id, Price new_price, Quantity new_quantity);
    
    MarketData get_market_data() const;
    
    // SIMD-optimized price calculations
    Price calculate_vwap(Side side, Quantity quantity) const;
    
private:
    std::vector<Trade> match_order(Order* order);
    void execute_trade(Order* buy_order, Order* sell_order, Price price, Quantity quantity);
    
    // SIMD helper functions
    __m256i load_prices_simd(const std::vector<Price>& prices, size_t offset) const;
    Price calculate_weighted_price_simd(const std::vector<Price>& prices, 
                                       const std::vector<Quantity>& quantities) const;
};

} // namespace trading_engine::matching_engine