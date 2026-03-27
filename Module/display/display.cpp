
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Display {

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


std::string get_brightness()       { return run("brightnessctl get 2>&1 && echo '/' && brightnessctl max 2>&1"); }
std::string set_brightness(int pct) { return run("brightnessctl set " + std::to_string(pct) + "% 2>&1 && echo 'Brightness: " + std::to_string(pct) + "%'"); }
std::string brightness_up(int step) { return run("brightnessctl set +" + std::to_string(step) + "% 2>&1"); }
std::string brightness_down(int step) { return run("brightnessctl set " + std::to_string(step) + "%- 2>&1"); }


std::string list_monitors()     { return run("wlr-randr 2>&1 || xrandr 2>&1 | head -20"); }
std::string resolution()       { return run("wlr-randr 2>&1 | head -10 || xrandr 2>&1 | grep '*'"); }


std::string set_wallpaper(const std::string& path) { 
    return run("swaybg -i " + path + " -m fill 2>/dev/null & disown || feh --bg-fill " + path + " 2>&1 && echo 'Wallpaper set: " + path + "'"); 
}


std::string night_light_on()    { return run("gammastep -O 3500 2>/dev/null & echo 'Night light ON' || redshift -O 3500 2>&1"); }
std::string night_light_off()   { return run("pkill gammastep 2>/dev/null; pkill redshift 2>/dev/null; echo 'Night light OFF'"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "brightness") return get_brightness();
    if (action == "set_brightness") return set_brightness(std::stoi(args.empty() ? "50" : args));
    if (action == "bright_up") return brightness_up(10);
    if (action == "bright_down") return brightness_down(10);
    if (action == "monitors") return list_monitors();
    if (action == "resolution") return resolution();
    if (action == "wallpaper") return set_wallpaper(args);
    if (action == "night_on") return night_light_on();
    if (action == "night_off") return night_light_off();
    return "[Unknown display action: " + action + "]";
}

}
