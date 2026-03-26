#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class Utils {
public:
    static std::string to_lower(std::string s);
    static std::string escape_json(std::string_view s);
};

}
