
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Notify {

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


std::string send(const std::string& title, const std::string& body) {
    log_message(("NOTIFY: " + title + " - " + body).c_str());
    return run("notify-send '" + title + "' '" + body + "' 2>&1 && echo 'Notification sent'");
}

std::string send_urgent(const std::string& title, const std::string& body) {
    return run("notify-send -u critical '" + title + "' '" + body + "' 2>&1 && echo 'Urgent notification sent'");
}

std::string send_with_icon(const std::string& title, const std::string& body, const std::string& icon) {
    return run("notify-send -i '" + icon + "' '" + title + "' '" + body + "' 2>&1");
}

std::string send_timed(const std::string& title, const std::string& body, int ms) {
    return run("notify-send -t " + std::to_string(ms) + " '" + title + "' '" + body + "' 2>&1");
}

std::string clear_all() {
    return run("makoctl dismiss -a 2>/dev/null || dunstctl close-all 2>/dev/null && echo 'All notifications cleared'");
}

std::string history() {
    return run("makoctl list 2>/dev/null || dunstctl history 2>/dev/null | head -20");
}


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "send") return send("FreeMCP", args);
    if (action == "urgent") return send_urgent("FreeMCP ⚠️", args);
    if (action == "clear") return clear_all();
    if (action == "history") return history();
    return "[Unknown notify action: " + action + "]";
}

}
