#include "FreeMCP/Core/ToolExecutor.h"
#include "FreeMCP/Core/Utils.h"
#include "FreeMCP/Core/SystemDiscovery.h"
#include "FreeMCP/Persona.h"
#include <vector>
#include <array>
#include <cstdio>
#include <memory>
#include <string_view>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

bool ToolExecutor::kill_switch_enabled = false; 

void ToolExecutor::set_kill_switch(bool active) {
    kill_switch_enabled = active;
}

bool ToolExecutor::is_kill_switch_active() {
    return kill_switch_enabled;
}

std::string ToolExecutor::execute(std::string_view tool_call_raw) {
    if (kill_switch_enabled) {
        return "[BLOCKED: Kill Switch Active]";
    }
    std::string tool_call = std::string(tool_call_raw);
    auto is_junk = [](unsigned char c) { return std::isspace(c) || c == ' ' || c == '>' || c == '\n' || c == '\r' || c == '\t'; };
    while (!tool_call.empty() && is_junk(tool_call.front())) tool_call.erase(tool_call.begin());
    while (!tool_call.empty() && is_junk(tool_call.back())) tool_call.pop_back();
    if (tool_call.substr(0, 3) == "AI:") tool_call.erase(0, 3);
    while (!tool_call.empty() && is_junk(tool_call.front())) tool_call.erase(tool_call.begin());
    while (!tool_call.empty() && is_junk(tool_call.back())) tool_call.pop_back();

    size_t paren_open = tool_call.find('(');
    std::string tool_name;
    std::string args;
    
    std::vector<std::string> known_tools = {"terminal", "read", "write", "execute", "mode", "character", "remember", "recall", "type"};

    if (paren_open == std::string::npos) {
        size_t first_space = tool_call.find(' ');
        std::string first_word = (first_space == std::string::npos) ? tool_call : tool_call.substr(0, first_space);
        std::string lower_word = Utils::to_lower(first_word);
        
        bool is_known = false;
        for (const auto& kt : known_tools) if (kt == lower_word) { is_known = true; break; }

        if (is_known) {
            tool_name = lower_word;
            args = (first_space == std::string::npos) ? "" : tool_call.substr(first_space + 1);
        } else if (lower_word == "open" && first_space != std::string::npos) {
            tool_name = "terminal";
            args = tool_call.substr(first_space + 1);
        } else {
            tool_name = "terminal";
            args = tool_call;
        }
    } else {
        tool_name = tool_call.substr(0, paren_open);
        while (!tool_name.empty() && (tool_name.front() == ' ' || tool_name.front() == '\n')) tool_name.erase(tool_name.begin());
        while (!tool_name.empty() && (tool_name.back() == ' ' || tool_name.back() == '\n')) tool_name.pop_back();
        
        size_t paren_close = tool_call.rfind(')');
        if (paren_close != std::string::npos && paren_close > paren_open) {
            args = tool_call.substr(paren_open + 1, paren_close - paren_open - 1);
        } else {
            args = tool_call.substr(paren_open + 1);
        }
    }
    
    std::string norm_tool = Utils::to_lower(tool_name);
    
    
    if (norm_tool == "[read]") norm_tool = "read";
    else if (norm_tool == "[write]") norm_tool = "write";
    else if (norm_tool == "[list]") { norm_tool = "terminal"; if (args.empty()) args = "ls"; else args = "ls " + args; }
    else if (norm_tool == "[exec]" || norm_tool == "[terminal]") norm_tool = "terminal";
    
    log_message(("AI ACTION: " + norm_tool + " -> " + args).c_str());
    
    if (norm_tool == "terminal" || norm_tool == "execute" || norm_tool == "tool_call" || norm_tool == "tool") {
        std::string result;
        std::array<char, 512> buffer;
        std::string cmd = args;
        
        if (args.find("terminal") != std::string::npos || args.find("kitty") != std::string::npos || norm_tool == "terminal") {
             std::string term = SystemDiscovery::get_terminal();
             std::string term_cmd = args;
             
             
             if (norm_tool == "terminal" && !args.empty() && args != "terminal") {
                 term_cmd = term + " sh -c \"" + args + "; exec zsh\"";
             } else if (args == "terminal" || args.empty()) {
                 term_cmd = term;
             } else {
                 
                 size_t pos = term_cmd.find("terminal");
                 while (pos != std::string::npos) {
                     term_cmd.replace(pos, 8, term);
                     pos = term_cmd.find("terminal", pos + term.length());
                 }
             }

             cmd = "(setsid " + term_cmd + ") > /dev/null 2>&1 &";
             log_message(("EXECUTING VISIBLE TERMINAL: " + cmd).c_str());
             system(cmd.c_str());
             return "[Terminal Window Started: " + term + "]";
        }
        
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) return "[Execution Error]";
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        if (result.empty()) result = "[Success (No Output)]";
        return result;
    } else if (norm_tool == "read") {
        std::string result;
        std::array<char, 512> buffer;
        std::string cmd = "cat " + args + " 2>&1";
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) return "[Read Error]";
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    } else if (norm_tool == "type") {
        std::string clean = args;
        
        for (char &c : clean) if (c == ';' || c == '|' || c == '&' || c == '>') c = ' ';
        std::string type_cmd = "xdotool type --delay 50 \"" + clean + "\"";
        system(type_cmd.c_str());
        return "[Typed: " + clean + "]";
    } else if (norm_tool == "write") {
        size_t comma = args.find(',');
        if (comma == std::string::npos) return "[Write Error: format path,content]";
        std::string path = args.substr(0, comma);
        std::string content = args.substr(comma + 1);
        while (!path.empty() && path.front() == ' ') path.erase(path.begin());
        while (!content.empty() && content.front() == ' ') content.erase(content.begin());
        std::string cmd = "echo '" + content + "' > " + path;
        system(cmd.c_str());
        return "[Written]";
    } else if (norm_tool == "mode") {
        std::string m = Utils::to_lower(args);
        while (!m.empty() && (m.front() == ' ' || m.front() == '\n' || m.front() == '"' || m.front() == '\'')) m.erase(m.begin());
        while (!m.empty() && (m.back() == ' ' || m.back() == '\n' || m.back() == '"' || m.back() == '\'')) m.pop_back();
        
        if (m == "chill") {
            Persona::set_mode(Persona::Mode::CHILL);
            return "[Mode set to CHILL]";
        } else if (m == "productive") {
            Persona::set_mode(Persona::Mode::PRODUCTIVE);
            return "[Mode set to PRODUCTIVE]";
        } else if (m == "hackermode") {
            Persona::set_mode(Persona::Mode::HACKERMODE);
            return "[Mode set to HACKERMODE]";
        }
        return "[Unknown Mode: " + m + "]";
    } else if (norm_tool == "character") {
        std::string c = Utils::to_lower(args);
        while (!c.empty() && (c.front() == ' ' || c.front() == '\n' || c.front() == '"' || c.front() == '\'')) c.erase(c.begin());
        while (!c.empty() && (c.back() == ' ' || c.back() == '\n' || c.back() == '"' || c.back() == '\'')) c.pop_back();
        
        if (c == "light") {
            Persona::set_character(Persona::Character::LIGHT);
            return "[Character set to LIGHT]";
        } else if (c == "ace") {
            Persona::set_character(Persona::Character::ACE);
            return "[Character set to ACE]";
        }
        return "[Unknown Character: " + c + ". Available: ACE, LIGHT]";
    }
    
    return "[Unknown Tool: " + norm_tool + "]";
}

std::string ToolExecutor::process_all(std::string_view response) {
    std::string result = std::string(response);
    std::string final_output = "";
    
    size_t start = 0;
    size_t end = result.find('\n');
    while (start != std::string::npos) {
        std::string line = (end == std::string::npos) ? result.substr(start) : result.substr(start, end - start);
        
        std::string trimmed = line;
        while (!trimmed.empty() && (trimmed.front() == ' ' || trimmed.front() == '\r')) trimmed.erase(trimmed.begin());
        std::string lower = Utils::to_lower(trimmed);

        if (lower.find("do :") == 0) {
            std::string action = trimmed.substr(4);
            while (!action.empty() && action.front() == ' ') action.erase(action.begin());
            std::string output = execute(action);
            final_output += "\n> " + action + "\n" + output + "\n";
        } else if (lower.find("tell :") == 0) {
            std::string msg = trimmed.substr(6);
            while (!msg.empty() && msg.front() == ' ') msg.erase(msg.begin());
            final_output += msg + "\n";
        } else if (!trimmed.empty() && trimmed != "[TASK_COMPLETE]") {
            
            
            if (trimmed.front() != '>' && trimmed.front() != '[' 
                && trimmed.find("Success") == std::string::npos
                && trimmed.find("Execution") == std::string::npos) {
                final_output += trimmed + "\n";
            }
        }

        if (end == std::string::npos) break;
        start = end + 1;
        end = result.find('\n', start);
    }
    
    return final_output;
}

}
