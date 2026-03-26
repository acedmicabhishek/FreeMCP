#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

class ToolExecutor {
public:
    static std::string execute(std::string_view tool_call_raw);
    static std::string process_all(std::string_view response);
};

}
