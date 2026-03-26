#pragma once
#include <string>
#include <vector>
#include <string_view>

namespace FreeMCP::Core {

class OllamaClient {
public:
    static std::vector<std::string> list_models();
    static bool pull_model(std::string_view name);
    static bool remove_model(std::string_view name);
    static bool run_model(std::string_view name);
    static bool ensure_service_active();
};

}
