#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class PromptBuilder {
public:
    static std::string build(std::string_view user_prompt);
};

}
