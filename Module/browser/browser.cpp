#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Browser {


static std::string run(const std::string& cmd) {
    std::string result;
    std::array<char, 512> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "[Error]";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    while (!result.empty() && (result.back() == '\n' || result.back() == ' '))
        result.pop_back();
    return result;
}



std::string open_url(const std::string& url) {
    log_message(("BROWSER: Opening URL: " + url).c_str());
    std::string cmd = "xdg-open '" + url + "' 2>/dev/null &";
    system(cmd.c_str());
    return "[Opened: " + url + "]";
}

std::string search(const std::string& query) {
    std::string url = "https:
    
    size_t pos = 0;
    while ((pos = url.find(' ', pos)) != std::string::npos) {
        url.replace(pos, 1, "+");
        pos++;
    }
    return open_url(url);
}



std::string mouse_move(int x, int y) {
    log_message(("BROWSER: Mouse move to " + std::to_string(x) + "," + std::to_string(y)).c_str());
    return run("xdotool mousemove " + std::to_string(x) + " " + std::to_string(y));
}

std::string mouse_click(int button) {
    log_message(("BROWSER: Mouse click button " + std::to_string(button)).c_str());
    return run("xdotool click " + std::to_string(button));
}

std::string mouse_click_at(int x, int y, int button) {
    mouse_move(x, y);
    return mouse_click(button);
}

std::string mouse_double_click() {
    return run("xdotool click --repeat 2 --delay 100 1");
}

std::string mouse_right_click() {
    return run("xdotool click 3");
}

std::string mouse_scroll(const std::string& direction, int clicks) {
    int btn = (direction == "up") ? 4 : 5;
    return run("xdotool click --repeat " + std::to_string(clicks) + " " + std::to_string(btn));
}

std::string get_mouse_position() {
    return run("xdotool getmouselocation --shell 2>&1");
}



std::string type_text(const std::string& text) {
    log_message(("BROWSER: Typing: " + text).c_str());
    return run("xdotool type --clearmodifiers '" + text + "'");
}

std::string key_press(const std::string& key) {
    log_message(("BROWSER: Key press: " + key).c_str());
    return run("xdotool key " + key);
}

std::string key_combo(const std::string& combo) {
    
    log_message(("BROWSER: Key combo: " + combo).c_str());
    return run("xdotool key " + combo);
}



std::string get_active_window() {
    return run("xdotool getactivewindow getwindowname 2>&1");
}

std::string focus_window(const std::string& name) {
    log_message(("BROWSER: Focus window: " + name).c_str());
    return run("xdotool search --name '" + name + "' windowactivate 2>&1");
}

std::string minimize_window() {
    return run("xdotool getactivewindow windowminimize");
}

std::string maximize_window() {
    return run("wmctrl -r :ACTIVE: -b add,maximized_vert,maximized_horz 2>&1");
}

std::string close_window() {
    return run("xdotool getactivewindow windowclose");
}



std::string screenshot(const std::string& path) {
    std::string target = path.empty() ? "/tmp/freemcp_screenshot.png" : path;
    log_message(("BROWSER: Screenshot → " + target).c_str());
    return run("grim " + target + " 2>&1 && echo 'Screenshot saved: " + target + "'");
}

std::string screenshot_region(int x, int y, int w, int h, const std::string& path) {
    std::string target = path.empty() ? "/tmp/freemcp_region.png" : path;
    std::string region = std::to_string(x) + "," + std::to_string(y) + " " + std::to_string(w) + "x" + std::to_string(h);
    return run("grim -g '" + region + "' " + target + " 2>&1 && echo 'Region saved: " + target + "'");
}



std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "open_url") return open_url(args);
    if (action == "search") return search(args);
    if (action == "click") return mouse_click(1);
    if (action == "right_click") return mouse_right_click();
    if (action == "double_click") return mouse_double_click();
    if (action == "type") return type_text(args);
    if (action == "key") return key_press(args);
    if (action == "combo") return key_combo(args);
    if (action == "screenshot") return screenshot(args);
    if (action == "active_window") return get_active_window();
    if (action == "focus") return focus_window(args);
    if (action == "minimize") return minimize_window();
    if (action == "maximize") return maximize_window();
    if (action == "close") return close_window();
    if (action == "scroll_up") return mouse_scroll("up", 3);
    if (action == "scroll_down") return mouse_scroll("down", 3);
    if (action == "mouse_pos") return get_mouse_position();
    return "[Unknown browser action: " + action + "]";
}

}
