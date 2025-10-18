#include <iostream>

// Stub implementation for CI build
namespace trading_engine::core {

class ConfigManager {
public:
    void load_config(const std::string& path) {
        std::cout << "Loading config from: " << path << std::endl;
    }
};

} // namespace trading_engine::core