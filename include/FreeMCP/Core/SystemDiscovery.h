#pragma once
#include <string>

namespace FreeMCP::Core {

class SystemDiscovery {
public:
    static void discover();
    static std::string get_os();
    static std::string get_terminal();
    static std::string get_context();

private:
    static std::string detected_os;
    static std::string detected_terminal;
};

}
