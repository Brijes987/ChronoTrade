#include <iostream>

// Stub implementation for CI build
namespace trading_engine::matching_engine {

class MatchingEngine {
public:
    void start() {
        std::cout << "Matching engine started" << std::endl;
    }
    
    void stop() {
        std::cout << "Matching engine stopped" << std::endl;
    }
};

} // namespace trading_engine::matching_engine