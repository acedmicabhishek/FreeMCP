#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class ChatEngine {
public:
    static std::string chat(std::string_view model, std::string_view prompt);
    static std::string chat_execute(std::string_view model, std::string_view prompt);
    static std::string chat_auto(std::string_view model, std::string_view prompt);
};

}
