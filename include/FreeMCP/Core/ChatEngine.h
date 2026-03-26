#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class ChatEngine {
public:
    static std::string chat(std::string_view model, std::string_view prompt);
};

}
