#include "FreeMCP/Core/ToolExecutor.h"
#include "FreeMCP/Core/Utils.h"
#include "FreeMCP/Persona.h"
#include "FreeMCP/Memory.h"
#include <vector>
#include <array>
#include <cstdio>
#include <memory>
#include <string_view>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

std::string ToolExecutor::execute(std::string_view tool_call_raw) {
    std::string tool_call = std::string(tool_call_raw);
    while (!tool_call.empty() && (tool_call.front() == ' ' || tool_call.front() == '>' || tool_call.front() == '\n')) tool_call.erase(tool_call.begin());
    if (tool_call.substr(0, 3) == "AI:") tool_call.erase(0, 3);
    while (!tool_call.empty() && (tool_call.front() == ' ' || tool_call.front() == '\n')) tool_call.erase(tool_call.begin());

    size_t paren_open = tool_call.find('(');
    std::string tool_name;
    std::string args;
    
    std::vector<std::string> known_tools = {"terminal", "read", "write", "execute", "mode", "character", "remember", "recall"};

    if (paren_open == std::string::npos) {
        size_t first_space = tool_call.find(' ');
        std::string first_word = (first_space == std::string::npos) ? tool_call : tool_call.substr(0, first_space);
        std::string lower_word = Utils::to_lower(first_word);
        
        bool is_known = false;
        for (const auto& kt : known_tools) if (kt == lower_word) { is_known = true; break; }

        if (is_known) {
            tool_name = lower_word;
            args = (first_space == std::string::npos) ? "" : tool_call.substr(first_space + 1);
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
    log_message(("AI ACTION: " + norm_tool + " -> " + args).c_str());
    
    if (norm_tool == "terminal" || norm_tool == "execute" || norm_tool == "tool_call" || norm_tool == "tool") {
        std::string result;
        std::array<char, 512> buffer;
        std::string cmd = args;
        
        if (args.find("kitty") != std::string::npos || args.find("terminal") != std::string::npos) {
             cmd = "(" + args + ") > /dev/null 2>&1 &";
             system(cmd.c_str());
             return "[Background Process Started]";
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
        } else if (c == "jarvis") {
            Persona::set_character(Persona::Character::JARVIS);
            return "[Character set to JARVIS]";
        } else if (c == "friday") {
            Persona::set_character(Persona::Character::FRIDAY);
            return "[Character set to FRIDAY]";
        } else if (c == "partner" || c == "girlfriend" || c == "boyfriend") {
            Persona::set_character(Persona::Character::PARTNER);
            return "[Character set to PARTNER]";
        } else if (c == "friend" || c == "cool_friend") {
            Persona::set_character(Persona::Character::COOL_FRIEND);
            return "[Character set to COOL_FRIEND]";
        }
        return "[Unknown Character: " + c + "]";
    } else if (norm_tool == "remember") {
        Memory::store(args);
        return "[Fact Stored in Long-Term Memory]";
    } else if (norm_tool == "recall") {
        std::vector<std::string> memories = Memory::recall(args);
        if (memories.empty()) return "[No relevant memories found]";
        std::string result = "[RECALLED MEMORIES]:\n";
        for (const auto& m : memories) result += "- " + m + "\n";
        return result;
    }
    
    return "[Unknown Tool: " + norm_tool + "]";
}

std::string ToolExecutor::process_all(std::string_view response) {
    std::string result = std::string(response);
    std::string final_output = "";
    size_t pos = 0;
    
    std::vector<std::string> start_tags = {"[TOOL_CALL]", "[tool_call]", "[TOOL]", "[tool]", "[TOOL_LIST]", "[toollist]"};
    std::vector<std::string> end_tags = {"[/TOOL_CALL]", "[/tool_call]", "[/TOOL]", "[/tool]", "[/TOOL_LIST]", "[/toollist]"};

    while (pos < result.length()) {
        size_t best_start = std::string::npos;
        size_t start_tag_len = 0;
        
        for (const auto& st : start_tags) {
            size_t found = result.find(st, pos);
            if (found != std::string::npos && (best_start == std::string::npos || found < best_start)) {
                best_start = found;
                start_tag_len = st.length();
            }
        }

        size_t best_end = std::string::npos;
        size_t end_tag_len = 0;
        for (const auto& et : end_tags) {
            size_t found = result.find(et, pos);
            if (found != std::string::npos && (best_end == std::string::npos || found < best_end)) {
                best_end = found;
                end_tag_len = et.length();
            }
        }

        if (best_start == std::string::npos && best_end == std::string::npos) {
            final_output += result.substr(pos);
            break;
        }

        if (best_start != std::string::npos && (best_end == std::string::npos || best_start < best_end)) {
            final_output += result.substr(pos, best_start - pos);
            
            size_t matching_end = std::string::npos;
            size_t matching_end_len = 0;
            for (const auto& et : end_tags) {
                size_t found = result.find(et, best_start + start_tag_len);
                if (found != std::string::npos && (matching_end == std::string::npos || found < matching_end)) {
                    matching_end = found;
                    matching_end_len = et.length();
                }
            }
            
            if (matching_end != std::string::npos) {
                std::string tool_call = result.substr(best_start + start_tag_len, matching_end - (best_start + start_tag_len));
                std::string tool_output = execute(tool_call);
                final_output += "\n> " + tool_call + "\n" + tool_output + "\n";
                pos = matching_end + matching_end_len;
            } else {
                final_output += result.substr(best_start, start_tag_len);
                pos = best_start + start_tag_len;
            }
        } else {
            final_output += result.substr(pos, best_end - pos);
            std::string possible_call = result.substr(pos, best_end - pos);
            size_t last_newline = possible_call.find_last_of('\n');
            size_t call_start = (last_newline == std::string::npos) ? 0 : last_newline + 1;
            
            std::string tool_call = possible_call.substr(call_start);
            std::string tool_output = execute(tool_call);
            
            final_output.erase(final_output.length() - tool_call.length());
            final_output += "\n> " + tool_call + "\n" + tool_output + "\n";
            pos = best_end + end_tag_len;
        }
    }
    return final_output;
}

}
