#include "FreeMCP/Core/SystemDiscovery.h"
#include <cstdio>
#include <memory>
#include <sstream>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

std::string SystemDiscovery::detected_os = "Unknown Linux";
std::string SystemDiscovery::detected_terminal = "x-terminal-emulator";

void SystemDiscovery::discover() {
    log_message("Starting System Discovery...");
    
    std::unique_ptr<FILE, decltype(&pclose)> os_pipe(popen("grep PRETTY_NAME /etc/os-release | cut -d'\"' -f2", "r"), pclose);
    if (os_pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), os_pipe.get())) {
            detected_os = buffer;
            if (!detected_os.empty() && detected_os.back() == '\n') detected_os.pop_back();
        }
    }
    
    const char* terminal_list[] = {"kitty", "gnome-terminal", "xfce4-terminal", "konsole", "xterm", "alacritty", "terminator"};
    for (const char* t : terminal_list) {
        std::string check_cmd = "which " + std::string(t) + " 2>/dev/null";
        std::unique_ptr<FILE, decltype(&pclose)> term_pipe(popen(check_cmd.c_str(), "r"), pclose);
        if (term_pipe) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), term_pipe.get())) {
                detected_terminal = buffer;
                if (!detected_terminal.empty() && detected_terminal.back() == '\n') detected_terminal.pop_back();
                break;
            }
        }
    }
    
    log_message(("Detected OS: " + detected_os).c_str());
    log_message(("Detected Primary Terminal: " + detected_terminal).c_str());
    log_message("System Discovery Complete.");
}

std::string SystemDiscovery::get_os() {
    return detected_os;
}

std::string SystemDiscovery::get_terminal() {
    return detected_terminal;
}

std::string SystemDiscovery::get_context() {
    std::stringstream ss;
    ss << "System Context for AI Agent:\n";
    ss << "- OS Environment: " << detected_os << "\n";
    ss << "- Available Terminal: " << detected_terminal << "\n";
    ss << "- CRITICAL: To launch GUI apps like the terminal, ALWAYS use: terminal(setsid " << detected_terminal << " &)\n";
    return ss.str();
}

}
