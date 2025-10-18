#include <iostream>

// Stub implementation for CI build
namespace trading_engine::core {

class Logger {
public:
    static void info(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    static void error(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
};

} // namespace trading_engine::core