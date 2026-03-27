#include "FreeMCP/Core/ConfigManager.h"
#include <iostream>

namespace FreeMCP::Core {

std::string ConfigManager::get_config_path() {
    char* home = getenv("HOME");
    std::string path = std::string(home) + "/.config/freemcp";
    std::filesystem::create_directories(path);
    return path + "/settings.conf";
}

void ConfigManager::save(const std::string& key, const std::string& value) {
    std::string path = get_config_path();
    std::ofstream out(path, std::ios::app);
    out << key << "=" << value << "\n";
}

std::string ConfigManager::load(const std::string& key, const std::string& default_value) {
    std::ifstream in(get_config_path());
    std::string line;
    std::string last_val = default_value;
    while (std::getline(in, line)) {
        if (line.find(key + "=") == 0) {
            last_val = line.substr(key.length() + 1);
        }
    }
    return last_val;
}

}
