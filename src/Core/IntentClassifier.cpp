#include "FreeMCP/Core/IntentClassifier.h"
#include "FreeMCP/Core/Utils.h"
#include "FreeMCP/Core/SystemDiscovery.h"
#include <vector>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {


static bool has_keyword(const std::string& input, const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (input.find(kw) != std::string::npos) return true;
    }
    return false;
}


static std::string extract_arg_after(const std::string& input, const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        size_t pos = input.find(kw);
        if (pos != std::string::npos) {
            std::string after = input.substr(pos + kw.length());
            
            while (!after.empty() && after.front() == ' ') after.erase(after.begin());
            
            size_t end = after.find_first_of(" \t\n\r");
            if (end != std::string::npos) after = after.substr(0, end);
            
            while (!after.empty() && (after.back() == '.' || after.back() == '!' || after.back() == '?'))
                after.pop_back();
            return after;
        }
    }
    return "";
}


static int extract_number(const std::string& input) {
    for (size_t i = 0; i < input.size(); i++) {
        if (std::isdigit(input[i])) {
            int num = 0;
            while (i < input.size() && std::isdigit(input[i])) {
                num = num * 10 + (input[i] - '0');
                i++;
            }
            return num;
        }
    }
    return -1; 
}

IntentResult IntentClassifier::classify(std::string_view user_input) {
    std::string input = Utils::to_lower(std::string(user_input));
    
    
    while (!input.empty() && input.front() == ' ') input.erase(input.begin());
    while (!input.empty() && input.back() == ' ') input.pop_back();

    
    
    
    if (input.find(" and ") != std::string::npos || 
        input.find(" then ") != std::string::npos ||
        input.find(" & ") != std::string::npos ||
        input.find("; ") != std::string::npos ||
        has_keyword(input, {"write", "create ", "build", "debug", "fix", "explain", "analyze", "why", "how to", "script"})) {
        return {false, "", ""}; 
    }

    
    
    
    if (has_keyword(input, {"open terminal", "launch terminal", "start terminal", "run terminal"})) {
        std::string term = SystemDiscovery::get_terminal();
        return {true, "(setsid " + term + ") > /dev/null 2>&1 &", "Opening terminal"};
    }
    
    if (has_keyword(input, {"open ", "launch ", "start ", "run "})) {
        std::string app = extract_arg_after(input, {"open ", "launch ", "start ", "run "});
        if (!app.empty() && (app == "browser" || app == "web" || app == "firefox" || 
                             app == "files" || app == "thunar" || 
                             app == "code" || app == "terminal" || app == "kitty")) {
            
            if (app == "browser" || app == "web") app = "firefox";
            else if (app == "files") app = "thunar";
            else if (app == "terminal" || app == "kitty") {
                std::string term = SystemDiscovery::get_terminal();
                return {true, "(setsid " + term + ") > /dev/null 2>&1 &", "Opening terminal"};
            }
            return {true, "(setsid " + app + ") > /dev/null 2>&1 &", "Launching " + app};
        }
    }


    
    
    
    if (has_keyword(input, {"update", "upgrade", "pacman -syu", "system update"})) {
        return {true, "sudo pacman -Syyu --noconfirm 2>&1", "Updating system with pacman"};
    }

    
    
    
    if (has_keyword(input, {"install "})) {
        std::string pkg = extract_arg_after(input, {"install "});
        if (!pkg.empty() && pkg != "a" && pkg != "the") {
            return {true, "sudo pacman -S " + pkg + " --noconfirm 2>&1", "Installing " + pkg};
        }
    }

    
    
    
    if (has_keyword(input, {"uninstall ", "remove "})) {
        std::string pkg = extract_arg_after(input, {"uninstall ", "remove "});
        if (!pkg.empty() && pkg != "a" && pkg != "the") {
            return {true, "sudo pacman -R " + pkg + " --noconfirm 2>&1", "Removing " + pkg};
        }
    }

    
    
    
    if (has_keyword(input, {"shutdown", "power off", "poweroff"})) {
        return {true, "sudo shutdown now", "Shutting down"};
    }
    if (has_keyword(input, {"reboot", "restart system", "restart pc", "restart computer"})) {
        return {true, "sudo reboot", "Rebooting"};
    }

    
    
    
    if (has_keyword(input, {"kill "})) {
        std::string proc = extract_arg_after(input, {"kill "});
        if (!proc.empty()) {
            return {true, "pkill " + proc + " 2>&1", "Killing " + proc};
        }
    }

    
    
    
    if (has_keyword(input, {"list files", "show files", "ls", "list directory"})) {
        return {true, "ls -la 2>&1", "Listing files"};
    }
    if (has_keyword(input, {"disk", "storage", "free space", "disk space", "df"})) {
        return {true, "df -h 2>&1", "Checking disk usage"};
    }
    if (has_keyword(input, {"memory", "ram", "free -h"})) {
        return {true, "free -h 2>&1", "Checking memory"};
    }
    if (has_keyword(input, {"ip address", "my ip", "ip addr"})) {
        return {true, "ip addr show | grep 'inet ' | awk '{print $2}' | head -5 2>&1", "Checking IP"};
    }
    if (has_keyword(input, {"screenshot", "screen capture", "take screenshot"})) {
        return {true, "grim ~/screenshot_$(date +%s).png 2>&1 && echo 'Screenshot saved!'", "Taking screenshot"};
    }
    if (has_keyword(input, {"cpu usage", "cpu load", "system load", "processor load", "check cpu", "cpu info"})) {
        return {true, "top -bn1 | head -20 2>&1", "Checking CPU load"};
    }
    if (has_keyword(input, {"whoami", "username", "who am i"})) {
        return {true, "whoami 2>&1", "Getting username"};
    }
    if (has_keyword(input, {"uptime", "how long"})) {
        return {true, "uptime 2>&1", "Checking uptime"};
    }

    
    
    
    if (has_keyword(input, {"volume"})) {
        int pct = extract_number(input);
        if (pct >= 0 && pct <= 100) {
            return {true, "wpctl set-volume @DEFAULT_AUDIO_SINK@ " + std::to_string(pct) + "% 2>&1 && echo 'Volume: " + std::to_string(pct) + "%'", "🔊 Volume → " + std::to_string(pct) + "%"};
        }
        if (has_keyword(input, {"up", "increase", "raise", "higher", "louder", "more"})) {
            return {true, "wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+ 2>&1 && echo 'Volume up'", "⬆ Volume up"};
        }
        if (has_keyword(input, {"max", "full", "100"})) {
            return {true, "wpctl set-volume @DEFAULT_AUDIO_SINK@ 100% 2>&1 && echo 'Max volume'", "🔊 Volume 100%"};
        }
        if (has_keyword(input, {"down", "decrease", "lower", "reduce", "quieter", "less"})) {
            return {true, "wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%- 2>&1 && echo 'Volume down'", "⬇ Volume down"};
        }
        
        return {true, "wpctl get-volume @DEFAULT_AUDIO_SINK@ 2>&1", "🔊 Current volume"};
    }
    if (has_keyword(input, {"mute", "unmute", "toggle mute"})) {
        return {true, "wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle 2>&1 && echo 'Mute toggled'", "🔇 Mute toggle"};
    }
    if (has_keyword(input, {"play pause", "play/pause", "pause music", "play music", "resume music"})) {
        return {true, "playerctl play-pause 2>&1 && echo 'Play/Pause'", "⏯ Play/Pause"};
    }
    if (has_keyword(input, {"next track", "next song", "skip song", "skip track"})) {
        return {true, "playerctl next 2>&1 && echo 'Next track'", "⏭ Next track"};
    }
    if (has_keyword(input, {"previous track", "prev track", "prev song", "previous song"})) {
        return {true, "playerctl previous 2>&1 && echo 'Previous track'", "⏮ Previous track"};
    }
    if (has_keyword(input, {"now playing", "current song", "what song", "whats playing"})) {
        return {true, "playerctl metadata --format '{{artist}} - {{title}}' 2>&1", "🎵 Now playing"};
    }

    
    
    
    if (has_keyword(input, {"brightness"})) {
        int pct = extract_number(input);
        if (pct >= 0 && pct <= 100) {
            
            return {true, "brightnessctl set " + std::to_string(pct) + "% 2>&1 && echo 'Brightness: " + std::to_string(pct) + "%'", "☀ Brightness → " + std::to_string(pct) + "%"};
        }
        if (has_keyword(input, {"up", "increase", "raise", "higher", "brighter", "more"})) {
            return {true, "brightnessctl set +10% 2>&1", "☀ Brightness up"};
        }
        
        return {true, "brightnessctl set 10%- 2>&1", "🔅 Brightness down"};
    }
    
    if (has_keyword(input, {"dimmer", "dim the", "dim screen"})) {
        return {true, "brightnessctl set 10%- 2>&1", "🔅 Brightness down"};
    }
    if (has_keyword(input, {"night light on", "night mode on", "warm light"})) {
        return {true, "gammastep -O 3500 2>/dev/null & echo 'Night light ON'", "🌙 Night light ON"};
    }
    if (has_keyword(input, {"night light off", "night mode off"})) {
        return {true, "pkill gammastep 2>/dev/null; pkill redshift 2>/dev/null; echo 'Night light OFF'", "☀ Night light OFF"};
    }

    
    
    
    if (has_keyword(input, {"wifi list", "available wifi", "scan wifi", "wifi scan"})) {
        return {true, "nmcli device wifi list 2>&1 | head -15", "📡 WiFi networks"};
    }
    if (has_keyword(input, {"wifi off", "disable wifi"})) {
        return {true, "nmcli radio wifi off && echo 'WiFi OFF'", "📡 WiFi OFF"};
    }
    if (has_keyword(input, {"wifi on", "enable wifi"})) {
        return {true, "nmcli radio wifi on && echo 'WiFi ON'", "📡 WiFi ON"};
    }
    if (has_keyword(input, {"bluetooth on", "bt on", "enable bluetooth"})) {
        return {true, "bluetoothctl power on 2>&1", "🔵 Bluetooth ON"};
    }
    if (has_keyword(input, {"bluetooth off", "bt off", "disable bluetooth"})) {
        return {true, "bluetoothctl power off 2>&1", "🔵 Bluetooth OFF"};
    }
    if (has_keyword(input, {"bluetooth devices", "bt devices", "paired devices"})) {
        return {true, "bluetoothctl devices 2>&1", "🔵 BT devices"};
    }
    if (has_keyword(input, {"ping"})) {
        std::string host = extract_arg_after(input, {"ping "});
        if (host.empty()) host = "8.8.8.8";
        return {true, "ping -c 3 " + host + " 2>&1", "🏓 Ping " + host};
    }
    if (has_keyword(input, {"public ip", "external ip", "what is my ip"})) {
        return {true, "curl -s ifconfig.me 2>&1", "🌐 Public IP"};
    }
    if (has_keyword(input, {"ports", "open ports", "listening"})) {
        return {true, "ss -tulnp 2>&1 | head -20", "🔌 Open ports"};
    }
    if (has_keyword(input, {"speedtest", "speed test", "internet speed"})) {
        return {true, "speedtest-cli --simple 2>&1 || echo 'Install: sudo pacman -S speedtest-cli'", "🚀 Speed test"};
    }

    
    
    
    if (has_keyword(input, {"clipboard", "paste", "get clipboard"})) {
        return {true, "wl-paste 2>/dev/null || xclip -selection clipboard -o 2>/dev/null || echo '[Empty]'", "📋 Clipboard"};
    }
    if (has_keyword(input, {"copy to clipboard"})) {
        std::string text = extract_arg_after(input, {"copy to clipboard "});
        return {true, "echo '" + text + "' | wl-copy 2>/dev/null && echo 'Copied'", "📋 Copied"};
    }

    
    
    
    if (has_keyword(input, {"notify ", "notification ", "send notification"})) {
        std::string msg = extract_arg_after(input, {"notify ", "notification "});
        return {true, "notify-send 'FreeMCP' '" + msg + "' 2>&1 && echo 'Notification sent'", "🔔 Notification"};
    }
    if (has_keyword(input, {"clear notifications", "dismiss notifications"})) {
        return {true, "makoctl dismiss -a 2>/dev/null || dunstctl close-all 2>/dev/null && echo 'Cleared'", "🔕 Cleared"};
    }

    
    
    
    if (has_keyword(input, {"suspend", "sleep"})) {
        return {true, "systemctl suspend 2>&1 && echo 'Suspending...'", "💤 Suspend"};
    }
    if (has_keyword(input, {"hibernate"})) {
        return {true, "systemctl hibernate 2>&1 && echo 'Hibernating...'", "❄ Hibernate"};
    }
    if (has_keyword(input, {"lock screen", "lock pc", "lock computer"})) {
        return {true, "loginctl lock-session 2>&1 || swaylock 2>/dev/null & echo 'Locked'", "🔒 Screen locked"};
    }
    if (has_keyword(input, {"battery", "charge", "battery level"})) {
        return {true, "cat /sys/class/power_supply/BAT*/capacity 2>/dev/null && echo '%' || echo 'No battery'", "🔋 Battery"};
    }

    
    
    
    if (has_keyword(input, {"click", "left click"})) {
        return {true, "xdotool click 1 && echo 'Clicked'", "🖱 Left click"};
    }
    if (has_keyword(input, {"right click"})) {
        return {true, "xdotool click 3 && echo 'Right clicked'", "🖱 Right click"};
    }
    if (has_keyword(input, {"double click"})) {
        return {true, "xdotool click --repeat 2 --delay 80 1 && echo 'Double clicked'", "🖱 Double click"};
    }
    if (has_keyword(input, {"scroll up"})) {
        return {true, "xdotool click --repeat 5 4 && echo 'Scrolled up'", "⬆ Scroll up"};
    }
    if (has_keyword(input, {"scroll down"})) {
        return {true, "xdotool click --repeat 5 5 && echo 'Scrolled down'", "⬇ Scroll down"};
    }
    if (has_keyword(input, {"mouse position", "cursor position", "where is cursor"})) {
        return {true, "xdotool getmouselocation --shell 2>&1", "🖱 Cursor position"};
    }
    if (has_keyword(input, {"center mouse", "center cursor"})) {
        return {true, "RES=$(xdpyinfo | awk '/dimensions/{print $2}'); W=${RES%x*}; H=${RES#*x}; xdotool mousemove $((W/2)) $((H/2)) && echo 'Centered'", "🖱 Centered"};
    }

    
    
    
    if (has_keyword(input, {"cat ", "read file", "show file"})) {
        std::string file = extract_arg_after(input, {"cat ", "read file ", "show file "});
        return {true, "cat " + file + " 2>&1 | head -50", "📄 Reading " + file};
    }
    if (has_keyword(input, {"tree", "dir tree", "directory tree"})) {
        return {true, "tree -L 2 2>&1 | head -30", "🌳 Directory tree"};
    }
    if (has_keyword(input, {"find ", "search file"})) {
        std::string name = extract_arg_after(input, {"find ", "search file "});
        return {true, "find . -name '*" + name + "*' 2>/dev/null | head -20", "🔍 Finding " + name};
    }

    
    
    
    if (has_keyword(input, {"running services", "list services", "active services"})) {
        return {true, "systemctl list-units --type=service --state=running | head -20", "⚙ Running services"};
    }
    if (has_keyword(input, {"processes", "top", "running processes"})) {
        return {true, "ps aux --sort=-%mem | head -15", "⚙ Top processes"};
    }

    
    
    
    if (has_keyword(input, {"type "})) {
        std::string text = extract_arg_after(input, {"type "});
        return {true, "xdotool type --clearmodifiers '" + text + "' && echo 'Typed'", "⌨ Typing"};
    }
    if (has_keyword(input, {"press enter", "hit enter"})) {
        return {true, "xdotool key Return && echo 'Enter pressed'", "⌨ Enter"};
    }
    if (has_keyword(input, {"press escape", "hit escape", "esc"})) {
        return {true, "xdotool key Escape && echo 'Escape pressed'", "⌨ Escape"};
    }
    if (has_keyword(input, {"press tab"})) {
        return {true, "xdotool key Tab && echo 'Tab pressed'", "⌨ Tab"};
    }
    if (has_keyword(input, {"alt tab", "switch window"})) {
        return {true, "xdotool key alt+Tab && echo 'Window switched'", "🪟 Alt+Tab"};
    }
    if (has_keyword(input, {"ctrl c", "copy"})) {
        return {true, "xdotool key ctrl+c && echo 'Ctrl+C'", "⌨ Ctrl+C"};
    }
    if (has_keyword(input, {"ctrl v"})) {
        return {true, "xdotool key ctrl+v && echo 'Ctrl+V'", "⌨ Ctrl+V"};
    }
    if (has_keyword(input, {"ctrl z", "undo"})) {
        return {true, "xdotool key ctrl+z && echo 'Undo'", "⌨ Ctrl+Z"};
    }
    if (has_keyword(input, {"ctrl s", "save"})) {
        return {true, "xdotool key ctrl+s && echo 'Saved'", "⌨ Ctrl+S"};
    }

    
    
    
    if (has_keyword(input, {"active window", "current window", "focused window"})) {
        return {true, "xdotool getactivewindow getwindowname 2>&1", "🪟 Active window"};
    }
    if (has_keyword(input, {"minimize window", "minimize"})) {
        return {true, "xdotool getactivewindow windowminimize && echo 'Minimized'", "🪟 Minimized"};
    }
    if (has_keyword(input, {"close window", "close app"})) {
        return {true, "xdotool getactivewindow windowclose && echo 'Closed'", "🪟 Closed"};
    }
    if (has_keyword(input, {"maximize window", "maximize", "fullscreen"})) {
        return {true, "wmctrl -r :ACTIVE: -b add,maximized_vert,maximized_horz 2>&1 && echo 'Maximized'", "🪟 Maximized"};
    }

    
    
    
    if (has_keyword(input, {"google ", "search ", "look up "})) {
        std::string query = extract_arg_after(input, {"google ", "search ", "look up "});
        std::string url = "https:
        return {true, "xdg-open '" + url + "' 2>/dev/null & echo 'Searching: " + query + "'", "🔍 Google: " + query};
    }

    
    
    
    log_message(("INTENT: No action match for: " + std::string(user_input)).c_str());
    return {false, "", ""};
}

}
