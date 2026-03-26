#include "FreeMCP/Core/ChatEngine.h"
#include "FreeMCP/Core/PromptBuilder.h"
#include "FreeMCP/Core/ToolExecutor.h"
#include "FreeMCP/Core/Utils.h"
#include <array>
#include <cstdio>
#include <memory>
#include <string_view>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

std::string ChatEngine::chat(std::string_view model, std::string_view prompt) {
    if (model.empty()) return "Error: No active model selected in Brain center.";
    
    std::string conversation_history = "\n\nUser: " + std::string(prompt);
    std::string total_response = "";
    int iterations = 0;
    const int MAX_ITERATIONS = 10;

    while (iterations < MAX_ITERATIONS) {
        std::string full_prompt = PromptBuilder::build(std::string(prompt)) + conversation_history + "\nAssistant:";
        
        std::string json_escaped = Utils::escape_json(full_prompt);

        std::string payload = "{\"model\": \"" + std::string(model) + "\", \"prompt\": \"" + json_escaped + "\", \"stream\": false}";
        std::string tmp_file = "/tmp/ollama_payload.json";
        FILE* fp = fopen(tmp_file.c_str(), "w");
        if (!fp) return "Error: File system failure.";
        fputs(payload.c_str(), fp);
        fclose(fp);

        std::string cmd = "curl -s -X POST http://localhost:11434/api/generate -d @" + tmp_file + " | jq -r .response";
        log_message(("Reasoning Loop " + std::to_string(iterations + 1) + " sent to " + std::string(model)).c_str());
        
        std::string raw_response;
        std::array<char, 512> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) { remove(tmp_file.c_str()); return "Error: Failed to reach Ollama."; }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { raw_response += buffer.data(); }
        remove(tmp_file.c_str());

        if (raw_response.empty() || raw_response == "null\n") return "Error: No response from model.";
        if (!raw_response.empty() && raw_response.back() == '\n') raw_response.pop_back();

        std::string processed = ToolExecutor::process_all(raw_response);
        total_response += processed + "\n";
        
        if (processed.find("[TASK_COMPLETE]") != std::string::npos) break;
        if (processed.find("> ") == std::string::npos) break;

        conversation_history += "\nAssistant: " + processed;
        iterations++;
    }

    return total_response;
}

}
