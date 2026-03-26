#include "FreeMCP/Core/Utils.h"
#include <algorithm>
#include <cctype>
#include <string_view>

namespace FreeMCP::Core {

std::string Utils::to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string Utils::escape_json(std::string_view s) {
    std::string escaped = "";
    for (char c : s) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else if ((unsigned char)c < 32) continue; 
        else escaped += c;
    }
    return escaped;
}

}
