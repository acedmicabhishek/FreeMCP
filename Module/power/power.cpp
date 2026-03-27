
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Power {

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


std::string shutdown()    { return run("sudo shutdown now 2>&1"); }
std::string reboot()      { return run("sudo reboot 2>&1"); }
std::string suspend()     { return run("systemctl suspend 2>&1 && echo 'Suspending...'"); }
std::string hibernate()   { return run("systemctl hibernate 2>&1 && echo 'Hibernating...'"); }
std::string lock_screen() { return run("loginctl lock-session 2>&1 || swaylock 2>/dev/null & echo 'Screen locked'"); }
std::string logout()      { return run("loginctl terminate-user $USER 2>&1 || swaymsg exit 2>&1"); }


std::string shutdown_timer(int minutes) { return run("sudo shutdown +" + std::to_string(minutes) + " 2>&1 && echo 'Shutdown in " + std::to_string(minutes) + " minutes'"); }
std::string cancel_shutdown() { return run("sudo shutdown -c 2>&1 && echo 'Shutdown cancelled'"); }


std::string battery_status() { return run("upower -i /org/freedesktop/UPower/devices/battery_BAT0 2>&1 | grep -E 'state|percentage|time' || cat /sys/class/power_supply/BAT*/capacity 2>/dev/null || echo 'No battery info'"); }
std::string power_profile()  { return run("powerprofilesctl get 2>&1 || echo 'power-profiles-daemon not installed'"); }
std::string set_profile(const std::string& profile) { return run("powerprofilesctl set " + profile + " 2>&1 && echo 'Profile: " + profile + "'"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "shutdown") return shutdown();
    if (action == "reboot") return reboot();
    if (action == "suspend") return suspend();
    if (action == "hibernate") return hibernate();
    if (action == "lock") return lock_screen();
    if (action == "logout") return logout();
    if (action == "timer") return shutdown_timer(std::stoi(args.empty() ? "30" : args));
    if (action == "cancel") return cancel_shutdown();
    if (action == "battery") return battery_status();
    if (action == "profile") return power_profile();
    if (action == "set_profile") return set_profile(args);
    return "[Unknown power action: " + action + "]";
}

}
