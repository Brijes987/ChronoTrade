#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>

#include "trading_engine/types.hpp"
#include "trading_engine/matching_engine/order_book.hpp"
#include "trading_engine/market_data/feed_handler.hpp"
#include "trading_engine/risk_management/risk_manager.hpp"
#include "trading_engine/networking/udp_receiver.hpp"
#include "trading_engine/grpc_api/trading_service.hpp"

using namespace trading_engine;

std::atomic<bool> running{true};

void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    running.store(false);
}

int main(int argc, char* argv[]) {
    std::cout << "Starting High-Performance Trading Engine..." << std::endl;
    
    // Setup signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    try {
        // Initialize core components
        matching_engine::OrderBook order_book("AAPL");
        market_data::FeedHandler feed_handler;
        risk_management::RiskManager risk_manager;
        networking::UdpReceiver market_data_receiver(9999);
        grpc_api::TradingService grpc_service;
        
        // Setup callbacks
        order_book.set_trade_callback([](const Trade& trade) {
            std::cout << "Trade executed: " << trade.symbol 
                     << " " << trade.quantity << "@" << trade.price << std::endl;
        });
        
        // Start services
        std::cout << "Starting market data feed..." << std::endl;
        auto feed_thread = std::jthread([&](std::stop_token stop_token) {
            feed_handler.start(stop_token);
        });
        
        std::cout << "Starting gRPC API server..." << std::endl;
        auto grpc_thread = std::jthread([&](std::stop_token stop_token) {
            grpc_service.start("0.0.0.0:50051", stop_token);
        });
        
        std::cout << "Trading engine started successfully!" << std::endl;
        std::cout << "Press Ctrl+C to shutdown..." << std::endl;
        
        // Main loop
        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Process any pending operations
            // This would typically involve processing market data,
            // risk checks, and order matching
        }
        
        std::cout << "Shutting down trading engine..." << std::endl;
        
        // Graceful shutdown
        feed_thread.request_stop();
        grpc_thread.request_stop();
        
        if (feed_thread.joinable()) feed_thread.join();
        if (grpc_thread.joinable()) grpc_thread.join();
        
        std::cout << "Trading engine shutdown complete." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}