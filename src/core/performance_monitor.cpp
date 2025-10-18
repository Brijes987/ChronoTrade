#include <iostream>

// Stub implementation for CI build
namespace trading_engine::core {

class PerformanceMonitor {
public:
    void start() {
        std::cout << "Performance monitor started" << std::endl;
    }
    
    void stop() {
        std::cout << "Performance monitor stopped" << std::endl;
    }
};

} // namespace trading_engine::core