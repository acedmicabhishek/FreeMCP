
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Clipboard {

static std::string run(const std::string& cmd) {
    std::string result;
    std::array<char, 512> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "[Error]";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    while (!result.empty() && (result.back() == '\n' || result.back() == ' '))
        result.pop_back();
    return result.empty() ? "[Done]" : result;
}


std::string get_clipboard()    { return run("wl-paste 2>/dev/null || xclip -selection clipboard -o 2>/dev/null || echo '[Empty]'"); }
std::string set_clipboard(const std::string& text) { 
    return run("echo '" + text + "' | wl-copy 2>/dev/null || echo '" + text + "' | xclip -selection clipboard 2>&1 && echo 'Copied to clipboard'"); 
}
std::string clear_clipboard()  { return run("wl-copy '' 2>/dev/null || echo '' | xclip -selection clipboard 2>&1 && echo 'Clipboard cleared'"); }
std::string clipboard_history() { return run("cliphist list 2>&1 | head -10 || echo 'cliphist not installed'"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "get") return get_clipboard();
    if (action == "set" || action == "copy") return set_clipboard(args);
    if (action == "clear") return clear_clipboard();
    if (action == "history") return clipboard_history();
    return "[Unknown clipboard action: " + action + "]";
}

}
