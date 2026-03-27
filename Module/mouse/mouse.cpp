#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>
#include <cmath>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Mouse {

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


std::string get_position()    { return run("xdotool getmouselocation --shell 2>&1"); }


std::string move_to(int x, int y) { 
    return run("xdotool mousemove " + std::to_string(x) + " " + std::to_string(y) + " && echo 'Cursor: " + std::to_string(x) + "," + std::to_string(y) + "'"); 
}


std::string move_relative(int dx, int dy) { 
    return run("xdotool mousemove_relative " + std::to_string(dx) + " " + std::to_string(dy)); 
}
std::string move_up(int px)    { return move_relative(0, -px); }
std::string move_down(int px)  { return move_relative(0, px); }
std::string move_left(int px)  { return move_relative(-px, 0); }
std::string move_right(int px) { return move_relative(px, 0); }


std::string smooth_move(int x, int y, int steps) {
    std::string script = "python3 -c \"\nimport subprocess, time\nresult = subprocess.run(['xdotool', 'getmouselocation', '--shell'], capture_output=True, text=True)\nlines = result.stdout.strip().split('\\n')\ncx = int(lines[0].split('=')[1])\ncy = int(lines[1].split('=')[1])\ntx, ty = " + std::to_string(x) + ", " + std::to_string(y) + "\nsteps = " + std::to_string(steps) + "\nfor i in range(steps+1):\n    nx = cx + (tx-cx)*i
    return run(script);
}


std::string click()           { return run("xdotool click 1 && echo 'Left click'"); }
std::string right_click()     { return run("xdotool click 3 && echo 'Right click'"); }
std::string middle_click()    { return run("xdotool click 2 && echo 'Middle click'"); }
std::string double_click()    { return run("xdotool click --repeat 2 --delay 80 1 && echo 'Double click'"); }
std::string triple_click()    { return run("xdotool click --repeat 3 --delay 80 1 && echo 'Triple click'"); }


std::string click_at(int x, int y)       { move_to(x, y); return click(); }
std::string right_click_at(int x, int y) { move_to(x, y); return right_click(); }


std::string drag(int x1, int y1, int x2, int y2) {
    return run("xdotool mousemove " + std::to_string(x1) + " " + std::to_string(y1) + 
               " mousedown 1 mousemove " + std::to_string(x2) + " " + std::to_string(y2) + 
               " mouseup 1 && echo 'Dragged'");
}


std::string scroll_up(int clicks)   { return run("xdotool click --repeat " + std::to_string(clicks) + " 4 && echo 'Scrolled up'"); }
std::string scroll_down(int clicks) { return run("xdotool click --repeat " + std::to_string(clicks) + " 5 && echo 'Scrolled down'"); }
std::string scroll_left(int clicks) { return run("xdotool click --repeat " + std::to_string(clicks) + " 6 && echo 'Scrolled left'"); }
std::string scroll_right(int clicks) { return run("xdotool click --repeat " + std::to_string(clicks) + " 7 && echo 'Scrolled right'"); }


std::string mouse_down(int btn) { return run("xdotool mousedown " + std::to_string(btn)); }
std::string mouse_up(int btn)   { return run("xdotool mouseup " + std::to_string(btn)); }


std::string center()          { return run("RES=$(xdpyinfo | awk '/dimensions/{print $2}'); W=${RES%x*}; H=${RES#*x}; xdotool mousemove $((W/2)) $((H/2)) && echo 'Centered'"); }
std::string top_left()       { return move_to(0, 0); }
std::string top_right()      { return run("RES=$(xdpyinfo | awk '/dimensions/{print $2}'); W=${RES%x*}; xdotool mousemove $W 0 && echo 'Top-right'"); }
std::string bottom_left()    { return run("RES=$(xdpyinfo | awk '/dimensions/{print $2}'); H=${RES#*x}; xdotool mousemove 0 $H && echo 'Bottom-left'"); }
std::string bottom_right()   { return run("RES=$(xdpyinfo | awk '/dimensions/{print $2}'); W=${RES%x*}; H=${RES#*x}; xdotool mousemove $W $H && echo 'Bottom-right'"); }

std::string type_text(const std::string& text) {
    if (text.empty()) return "[Error: No text to type]";
    
    std::string clean = text;
    for (char &c : clean) if (c == ';' || c == '|' || c == '&' || c == '>') c = ' ';
    return run("xdotool type --delay 50 \"" + clean + "\"");
}


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "type") return type_text(args);
    if (action == "pos") return get_position();
    if (action == "click") return click();
    if (action == "right") return right_click();
    if (action == "middle") return middle_click();
    if (action == "double") return double_click();
    if (action == "triple") return triple_click();
    if (action == "up") return move_up(50);
    if (action == "down") return move_down(50);
    if (action == "left") return move_left(50);
    if (action == "right_move") return move_right(50);
    if (action == "scroll_up") return scroll_up(3);
    if (action == "scroll_down") return scroll_down(3);
    if (action == "center") return center();
    if (action == "top_left") return top_left();
    if (action == "top_right") return top_right();
    if (action == "bottom_left") return bottom_left();
    if (action == "bottom_right") return bottom_right();
    if (action == "hold") return mouse_down(1);
    if (action == "release") return mouse_up(1);
    return "[Unknown mouse action: " + action + "]";
}

}
