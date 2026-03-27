#pragma once
#include <string>
#include <fstream>
#include <filesystem>

namespace FreeMCP::Core {

class ConfigManager {
public:
    static void save(const std::string& key, const std::string& value);
    static std::string load(const std::string& key, const std::string& default_value = "");

private:
    static std::string get_config_path();
};

}
