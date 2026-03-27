#pragma once
#include <string>

namespace FreeMCP::Module::Browser {
    std::string open_url(const std::string& url);
    std::string search(const std::string& query);
    std::string mouse_move(int x, int y);
    std::string mouse_click(int button = 1);
    std::string mouse_click_at(int x, int y, int button = 1);
    std::string mouse_double_click();
    std::string mouse_right_click();
    std::string mouse_scroll(const std::string& direction, int clicks = 3);
    std::string get_mouse_position();
    std::string type_text(const std::string& text);
    std::string key_press(const std::string& key);
    std::string key_combo(const std::string& combo);
    std::string get_active_window();
    std::string focus_window(const std::string& name);
    std::string minimize_window();
    std::string maximize_window();
    std::string close_window();
    std::string screenshot(const std::string& path = "");
    std::string screenshot_region(int x, int y, int w, int h, const std::string& path = "");
    std::string dispatch(const std::string& action, const std::string& args);
}
