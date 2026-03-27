
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Network {

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


std::string ip_address()       { return run("ip addr show | grep 'inet ' | awk '{print $2}' | head -5"); }
std::string public_ip()        { return run("curl -s ifconfig.me 2>&1"); }
std::string interfaces()      { return run("ip link show 2>&1"); }
std::string routing_table()   { return run("ip route 2>&1"); }
std::string dns_servers()     { return run("cat /etc/resolv.conf 2>&1"); }
std::string active_ports()    { return run("ss -tulnp 2>&1 | head -20"); }
std::string connections()     { return run("ss -tp 2>&1 | head -20"); }


std::string wifi_list()        { return run("nmcli device wifi list 2>&1 | head -15"); }
std::string wifi_connect(const std::string& ssid, const std::string& pass) {
    return run("nmcli device wifi connect '" + ssid + "' password '" + pass + "' 2>&1");
}
std::string wifi_disconnect()  { return run("nmcli device disconnect wlan0 2>&1"); }
std::string wifi_status()      { return run("nmcli connection show --active 2>&1"); }
std::string wifi_on()          { return run("nmcli radio wifi on && echo 'WiFi enabled'"); }
std::string wifi_off()         { return run("nmcli radio wifi off && echo 'WiFi disabled'"); }


std::string bt_on()            { return run("bluetoothctl power on 2>&1"); }
std::string bt_off()           { return run("bluetoothctl power off 2>&1"); }
std::string bt_scan()          { return run("timeout 5 bluetoothctl scan on 2>&1 && bluetoothctl devices 2>&1"); }
std::string bt_devices()       { return run("bluetoothctl devices 2>&1"); }
std::string bt_connect(const std::string& mac) { return run("bluetoothctl connect " + mac + " 2>&1"); }
std::string bt_disconnect(const std::string& mac) { return run("bluetoothctl disconnect " + mac + " 2>&1"); }


std::string ping(const std::string& host)       { return run("ping -c 3 " + host + " 2>&1"); }
std::string traceroute(const std::string& host)  { return run("traceroute " + host + " 2>&1 | head -15"); }
std::string speedtest()        { return run("speedtest-cli --simple 2>&1 || echo 'speedtest-cli not installed'"); }
std::string dns_lookup(const std::string& domain) { return run("dig +short " + domain + " 2>&1"); }


std::string fw_status()        { return run("sudo iptables -L -n 2>&1 | head -20"); }
std::string fw_allow(int port) { return run("sudo iptables -A INPUT -p tcp --dport " + std::to_string(port) + " -j ACCEPT && echo 'Port " + std::to_string(port) + " opened'"); }
std::string fw_block(int port) { return run("sudo iptables -A INPUT -p tcp --dport " + std::to_string(port) + " -j DROP && echo 'Port " + std::to_string(port) + " blocked'"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "ip") return ip_address();
    if (action == "public_ip") return public_ip();
    if (action == "interfaces") return interfaces();
    if (action == "ports") return active_ports();
    if (action == "connections") return connections();
    if (action == "wifi_list") return wifi_list();
    if (action == "wifi_status") return wifi_status();
    if (action == "wifi_on") return wifi_on();
    if (action == "wifi_off") return wifi_off();
    if (action == "wifi_disconnect") return wifi_disconnect();
    if (action == "bt_on") return bt_on();
    if (action == "bt_off") return bt_off();
    if (action == "bt_scan") return bt_scan();
    if (action == "bt_devices") return bt_devices();
    if (action == "bt_connect") return bt_connect(args);
    if (action == "bt_disconnect") return bt_disconnect(args);
    if (action == "ping") return ping(args.empty() ? "8.8.8.8" : args);
    if (action == "traceroute") return traceroute(args);
    if (action == "speedtest") return speedtest();
    if (action == "dns") return dns_lookup(args);
    if (action == "fw_status") return fw_status();
    return "[Unknown network action: " + action + "]";
}

}
