
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Audio {

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


std::string get_volume()       { return run("wpctl get-volume @DEFAULT_AUDIO_SINK@ 2>&1 || pactl get-sink-volume @DEFAULT_SINK@ 2>&1"); }
std::string set_volume(int pct) { return run("wpctl set-volume @DEFAULT_AUDIO_SINK@ " + std::to_string(pct) + "% 2>&1 || pactl set-sink-volume @DEFAULT_SINK@ " + std::to_string(pct) + "% 2>&1 && echo 'Volume: " + std::to_string(pct) + "%'"); }
std::string volume_up(int step) { return run("wpctl set-volume @DEFAULT_AUDIO_SINK@ " + std::to_string(step) + "%+ 2>&1 && echo 'Volume up " + std::to_string(step) + "%'"); }
std::string volume_down(int step) { return run("wpctl set-volume @DEFAULT_AUDIO_SINK@ " + std::to_string(step) + "%- 2>&1 && echo 'Volume down " + std::to_string(step) + "%'"); }
std::string mute()             { return run("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle 2>&1 && echo 'Mute toggled'"); }


std::string mic_mute()         { return run("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle 2>&1 && echo 'Mic mute toggled'"); }
std::string mic_volume(int pct) { return run("wpctl set-volume @DEFAULT_AUDIO_SOURCE@ " + std::to_string(pct) + "% 2>&1 && echo 'Mic: " + std::to_string(pct) + "%'"); }


std::string play_pause()       { return run("playerctl play-pause 2>&1 && echo 'Play/Pause toggled'"); }
std::string next_track()       { return run("playerctl next 2>&1 && echo 'Next track'"); }
std::string prev_track()       { return run("playerctl previous 2>&1 && echo 'Previous track'"); }
std::string now_playing()      { return run("playerctl metadata --format '{{artist}} - {{title}}' 2>&1"); }
std::string stop_playback()    { return run("playerctl stop 2>&1 && echo 'Stopped'"); }


std::string list_sinks()       { return run("wpctl status 2>&1 | head -30 || pactl list short sinks 2>&1"); }
std::string list_sources()     { return run("wpctl status 2>&1 | grep -A 10 'Sources' || pactl list short sources 2>&1"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "volume") return get_volume();
    if (action == "set_volume") return set_volume(std::stoi(args.empty() ? "50" : args));
    if (action == "up") return volume_up(5);
    if (action == "down") return volume_down(5);
    if (action == "mute") return mute();
    if (action == "mic_mute") return mic_mute();
    if (action == "play") return play_pause();
    if (action == "next") return next_track();
    if (action == "prev") return prev_track();
    if (action == "playing") return now_playing();
    if (action == "stop") return stop_playback();
    if (action == "sinks") return list_sinks();
    if (action == "sources") return list_sources();
    return "[Unknown audio action: " + action + "]";
}

}
