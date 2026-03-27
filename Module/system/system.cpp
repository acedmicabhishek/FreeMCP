#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::System {

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


std::string update_system()     { return run("sudo pacman -Syyu --noconfirm 2>&1"); }
std::string install(const std::string& pkg)  { return run("sudo pacman -S " + pkg + " --noconfirm 2>&1"); }
std::string remove(const std::string& pkg)   { return run("sudo pacman -R " + pkg + " --noconfirm 2>&1"); }
std::string search(const std::string& pkg)   { return run("pacman -Ss " + pkg + " 2>&1 | head -20"); }
std::string list_installed()    { return run("pacman -Q | wc -l"); }
std::string pkg_info(const std::string& pkg) { return run("pacman -Qi " + pkg + " 2>&1 | head -15"); }
std::string clean_cache()      { return run("sudo pacman -Scc --noconfirm 2>&1"); }
std::string install_aur(const std::string& pkg) { return run("yay -S " + pkg + " --noconfirm 2>&1"); }


std::string list_processes()    { return run("ps aux --sort=-%mem | head -15"); }
std::string kill_process(const std::string& name) { return run("pkill -f " + name + " 2>&1 && echo 'Killed: " + name + "'"); }
std::string kill_pid(int pid)   { return run("kill -9 " + std::to_string(pid) + " 2>&1"); }
std::string process_tree()     { return run("pstree -p | head -30"); }


std::string service_start(const std::string& svc)   { return run("sudo systemctl start " + svc + " 2>&1"); }
std::string service_stop(const std::string& svc)    { return run("sudo systemctl stop " + svc + " 2>&1"); }
std::string service_restart(const std::string& svc)  { return run("sudo systemctl restart " + svc + " 2>&1"); }
std::string service_status(const std::string& svc)   { return run("systemctl status " + svc + " 2>&1 | head -15"); }
std::string service_enable(const std::string& svc)   { return run("sudo systemctl enable " + svc + " 2>&1"); }
std::string service_disable(const std::string& svc)  { return run("sudo systemctl disable " + svc + " 2>&1"); }
std::string list_services()    { return run("systemctl list-units --type=service --state=running | head -20"); }


std::string hostname()         { return run("hostnamectl 2>&1"); }
std::string uptime()           { return run("uptime -p"); }
std::string kernel()           { return run("uname -r"); }
std::string cpu_info()         { return run("lscpu | head -15"); }
std::string memory()           { return run("free -h"); }
std::string disk_usage()       { return run("df -h | head -10"); }
std::string disk_detailed()    { return run("lsblk -f"); }
std::string gpu_info()         { return run("lspci | grep -i vga"); }
std::string temperature()      { return run("sensors 2>&1 | head -20"); }
std::string battery()          { return run("cat /sys/class/power_supply/BAT*/capacity 2>/dev/null || echo 'No battery'"); }
std::string users_logged_in()  { return run("who"); }
std::string os_info()          { return run("cat /etc/os-release | head -5"); }

std::string open_terminal(const std::string& args) {
    if (args.empty()) {
        system("setsid kitty &");
        return "Opened kitty terminal";
    } else {
        std::string cmd = "setsid kitty sh -c \"" + args + "; exec zsh\" &";
        system(cmd.c_str());
        return "Opened kitty with: " + args;
    }
}


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "terminal") return open_terminal(args);
    if (action == "update") return update_system();
    if (action == "install") return install(args);
    if (action == "remove") return remove(args);
    if (action == "search") return search(args);
    if (action == "installed") return list_installed();
    if (action == "pkg_info") return pkg_info(args);
    if (action == "clean") return clean_cache();
    if (action == "aur") return install_aur(args);
    if (action == "ps") return list_processes();
    if (action == "kill") return kill_process(args);
    if (action == "pstree") return process_tree();
    if (action == "svc_start") return service_start(args);
    if (action == "svc_stop") return service_stop(args);
    if (action == "svc_restart") return service_restart(args);
    if (action == "svc_status") return service_status(args);
    if (action == "svc_enable") return service_enable(args);
    if (action == "svc_disable") return service_disable(args);
    if (action == "services") return list_services();
    if (action == "hostname") return hostname();
    if (action == "uptime") return uptime();
    if (action == "kernel") return kernel();
    if (action == "cpu") return cpu_info();
    if (action == "memory") return memory();
    if (action == "disk") return disk_usage();
    if (action == "disk_full") return disk_detailed();
    if (action == "gpu") return gpu_info();
    if (action == "temp") return temperature();
    if (action == "battery") return battery();
    if (action == "users") return users_logged_in();
    if (action == "os") return os_info();
    return "[Unknown system action: " + action + "]";
}

}
