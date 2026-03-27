#include "FreeMCP/Core/ChatEngine.h"
#include "FreeMCP/Core/PromptBuilder.h"
#include "FreeMCP/Core/ToolExecutor.h"
#include "FreeMCP/Core/Utils.h"
#include <array>
#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>
#include <sstream>

extern "C" void log_message(const char* msg);

#include "FreeMCP/Core/SystemShell.h"
#include "FreeMCP/Core/ConfigManager.h"

namespace FreeMCP::Core {

std::string ChatEngine::chat(std::string_view model, std::string_view prompt) {
    std::string active_model = std::string(model);
    if (active_model.empty()) {
        active_model = ConfigManager::load("brain", "llama3.2:1b");
    }
    
    if (active_model.empty()) return "Error: No active model selected in Brain center.";
    
    std::string full_prompt = PromptBuilder::build(prompt);
    std::string json_escaped = Utils::escape_json(full_prompt);

    std::string payload = "{\"model\": \"" + active_model + "\", \"prompt\": \"" + json_escaped + "\", \"stream\": false}";
    std::string tmp_file = "/tmp/ollama_payload.json";
    FILE* fp = fopen(tmp_file.c_str(), "w");
    if (!fp) return "Error: File system failure.";
    fputs(payload.c_str(), fp);
    fclose(fp);

    std::string cmd = "curl -s -X POST http:
    log_message(("Chat sent to " + active_model).c_str());
    
    std::string raw_response;
    std::array<char, 512> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) { remove(tmp_file.c_str()); return "Error: Failed to reach Ollama."; }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { raw_response += buffer.data(); }
    remove(tmp_file.c_str());

    if (raw_response.empty() || raw_response == "null\n") return "Error: No response from model.";
    
    while (!raw_response.empty() && (raw_response.back() == '\n' || raw_response.back() == ' '))
        raw_response.pop_back();

    return raw_response;
}

std::string ChatEngine::chat_execute(std::string_view model, std::string_view prompt) {
    std::string active_model = std::string(model);
    if (active_model.empty()) {
        active_model = ConfigManager::load("brain", "llama3.2:1b");
    }
    if (active_model.empty()) return "Error: No active model.";

    
    std::string full_prompt = PromptBuilder::build_execute(prompt);
    std::string json_escaped = Utils::escape_json(full_prompt);

    std::string payload = "{\"model\": \"" + active_model + "\", \"prompt\": \"" + json_escaped + "\", \"stream\": false}";
    std::string tmp_file = "/tmp/ollama_exec_payload.json";
    FILE* fp = fopen(tmp_file.c_str(), "w");
    if (!fp) return "Error: File system failure.";
    fputs(payload.c_str(), fp);
    fclose(fp);

    std::string cmd = "curl -s -X POST http:
    log_message(("EXEC AI query sent to " + active_model).c_str());

    std::string raw_response;
    std::array<char, 512> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) { remove(tmp_file.c_str()); return "Error: Failed to reach Ollama."; }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) { raw_response += buffer.data(); }
    remove(tmp_file.c_str());

    if (raw_response.empty() || raw_response == "null\n") return "Error: No response from model.";
    while (!raw_response.empty() && (raw_response.back() == '\n' || raw_response.back() == ' '))
        raw_response.pop_back();
    while (!raw_response.empty() && (raw_response.front() == '\n' || raw_response.front() == ' '))
        raw_response.erase(raw_response.begin());

    
    log_message(("EXEC AI COMMAND: " + raw_response).c_str());

    std::string exec_output;
    std::unique_ptr<FILE, decltype(&pclose)> exec_pipe(popen(raw_response.c_str(), "r"), pclose);
    if (exec_pipe) {
        while (fgets(buffer.data(), buffer.size(), exec_pipe.get()) != nullptr)
            exec_output += buffer.data();
    }
    if (exec_output.empty()) exec_output = "[Done]";

    return "> " + raw_response + "\n" + exec_output;
}

std::string ChatEngine::chat_auto(std::string_view model, std::string_view prompt) {
    std::string active_model = std::string(model);
    if (active_model.empty()) active_model = ConfigManager::load("brain", "llama3.2:1b");
    if (active_model.empty()) return "Error: No active model.";

    std::string conversation_context = std::string(prompt);
    std::string final_combined_output = "";
    int max_steps = 10; 

    static SystemShell global_shell; 

    for (int step = 0; step < max_steps; ++step) {
        std::string current_prompt = PromptBuilder::build_auto(conversation_context);
        std::string json_escaped = Utils::escape_json(current_prompt);
        std::string payload = "{\"model\": \"" + active_model + "\", \"prompt\": \"" + json_escaped + "\", \"stream\": false, \"options\": {\"stop\": [\"[SYSTEM_RESULT]\", \"Observation:\"]}}";
        std::string tmp_file = "/tmp/ollama_loop_payload.json";
        
        FILE* f = fopen(tmp_file.c_str(), "w");
        if (f) { fwrite(payload.data(), 1, payload.size(), f); fclose(f); }

        std::string raw_response;
        std::array<char, 512> buffer;
        std::string curl_cmd = "curl -s -X POST http:
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(curl_cmd.c_str(), "r"), pclose);
        if (pipe) {
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) raw_response += buffer.data();
        }
        remove(tmp_file.c_str());

        if (raw_response.empty() || raw_response == "null\n") break;

        final_combined_output += "\n---\n" + raw_response + "\n";

        
        std::string prefix_bash = "```bash\n";
        std::string prefix_sh = "```sh\n";
        std::string suffix = "```";
        size_t start_pos = raw_response.find(prefix_bash);
        if (start_pos == std::string::npos) {
            start_pos = raw_response.find(prefix_sh);
            if (start_pos != std::string::npos) start_pos += prefix_sh.length();
        } else {
            start_pos += prefix_bash.length();
        }

        if (start_pos != std::string::npos) {
            size_t end_pos = raw_response.find(suffix, start_pos);
            if (end_pos != std::string::npos) {
                std::string cmd = raw_response.substr(start_pos, end_pos - start_pos);
                
                std::string clean_cmd; bool in_tag = false;
                for (char c : cmd) { if (c == '<') in_tag = true; else if (c == '>') { in_tag = false; continue; } if (!in_tag) clean_cmd += c; }
                cmd = clean_cmd;

                log_message(("AGENT STEP [" + std::to_string(step+1) + "]:\n" + cmd).c_str());
                
                std::string step_output = global_shell.execute_block(cmd, 30000);
                final_combined_output += "\n[System Execution Output]\n" + step_output + "\n";
                
                
                conversation_context += "\n" + raw_response + "\n[SYSTEM_RESULT]\nObservation: " + step_output + "\n";
                
                if (raw_response.find("[TASK_COMPLETE]") != std::string::npos) break;
            } else {
                conversation_context += "\n" + raw_response + "\n[SYSTEM_RESULT]\nObservation: Format Error - Missing closing ```\n";
                if (raw_response.find("[TASK_COMPLETE]") != std::string::npos) break;
            }
        } else {
            if (raw_response.find("[TASK_COMPLETE]") != std::string::npos) {
                break;
            } else {
                conversation_context += "\n" + raw_response + "\n[SYSTEM_RESULT]\nObservation: Format Error - No ```bash block found to execute.\n";
            }
        }
    }

    return final_combined_output;
}


}

