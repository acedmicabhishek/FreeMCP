#include "FreeMCP/Core/OllamaClient.h"
#include <cstdio>
#include <memory>
#include <array>
#include <cstdlib>
#include <string_view>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

std::vector<std::string> OllamaClient::list_models() {
    std::vector<std::string> models;
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("ollama list | tail -n +2", "r"), pclose);
    if (!pipe) return models;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        std::string line = buffer.data();
        size_t first_space = line.find_first_of(" \t");
        if (first_space != std::string::npos) {
            models.push_back(line.substr(0, first_space));
        }
    }
    return models;
}

bool OllamaClient::pull_model(std::string_view name) {
    std::string cmd = "ollama pull " + std::string(name);
    return system(cmd.c_str()) == 0;
}

bool OllamaClient::remove_model(std::string_view name) {
    std::string cmd = "ollama rm " + std::string(name);
    return system(cmd.c_str()) == 0;
}

bool OllamaClient::ensure_service_active() {
    if (system("systemctl is-active --quiet ollama") == 0) return true;
    log_message("Ollama service inactive. Requesting start...");
    if (system("pkexec systemctl start ollama") != 0) {
        log_message("ERROR: User denied elevation or pkexec failed.");
        return false;
    }
    log_message("Waiting for Ollama API to respond...");
    for (int i = 0; i < 20; ++i) {
        if (system("curl -s http://localhost:11434/api/tags > /dev/null") == 0) {
            log_message("Ollama service is now ACTIVE.");
            return true;
        }
        system("sleep 0.2");
    }
    log_message("ERROR: Timeout waiting for Ollama API.");
    return false;
}

bool OllamaClient::run_model(std::string_view name) {
    if (!ensure_service_active()) return false;
    log_message(("Triggering background load for: " + std::string(name)).c_str());
    std::string cmd = "ollama run " + std::string(name) + " \"\" &"; 
    return system(cmd.c_str()) >= 0;
}

}
