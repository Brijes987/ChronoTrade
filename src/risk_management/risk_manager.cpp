#include "trading_engine/risk_management/risk_manager.hpp"
#include <iostream>

namespace trading_engine::risk_management {

bool RiskManager::check_order_risk(const Order& order) {
    risk_checks_performed_.fetch_add(1);
    
    if (!check_order_size_limits(order)) {
        orders_rejected_.fetch_add(1);
        return false;
    }
    
    if (!check_position_limits(order.symbol, order)) {
        orders_rejected_.fetch_add(1);
        return false;
    }
    
    if (!check_daily_loss_limits()) {
        orders_rejected_.fetch_add(1);
        return false;
    }
    
    return true;
}

bool RiskManager::check_position_risk(const Symbol& symbol, const Trade& trade) {
    return true; // Stub implementation
}

bool RiskManager::check_portfolio_risk() {
    return true; // Stub implementation
}

void RiskManager::update_position(const Trade& trade) {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    
    auto& position = positions_[trade.symbol];
    if (trade.buy_order_id != 0) {
        position.long_position += trade.quantity;
    } else {
        position.short_position += trade.quantity;
    }
}

PositionInfo RiskManager::get_position(const Symbol& symbol) const {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    
    auto it = positions_.find(symbol);
    if (it != positions_.end()) {
        return it->second;
    }
    return PositionInfo{};
}

Price RiskManager::get_portfolio_value() const {
    return 0; // Stub implementation
}

void RiskManager::update_limits(const RiskLimits& new_limits) {
    limits_ = new_limits;
}

bool RiskManager::check_position_limits(const Symbol& symbol, const Order& order) {
    return true; // Stub implementation
}

bool RiskManager::check_order_size_limits(const Order& order) {
    return order.quantity <= limits_.max_order_size;
}

bool RiskManager::check_daily_loss_limits() {
    return daily_pnl_.load() > -limits_.max_daily_loss;
}

void RiskManager::trigger_circuit_breaker() {
    circuit_breaker_triggered_.store(true);
    std::cout << "Circuit breaker triggered!" << std::endl;
}

} // namespace trading_engine::risk_management