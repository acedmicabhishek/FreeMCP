#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class PromptBuilder {
public:
    static std::string build(std::string_view user_prompt);
    static std::string build_execute(std::string_view user_prompt);
    static std::string build_auto(std::string_view user_prompt);
    static void set_character_enabled(bool enabled);
    static bool is_character_enabled();
};

}
