#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class ToolExecutor {
public:
    static std::string execute(std::string_view tool_call_raw);
    static std::string process_all(std::string_view response);
    static void set_kill_switch(bool active);
    static bool is_kill_switch_active();
private:
    static bool kill_switch_enabled;
};

}
