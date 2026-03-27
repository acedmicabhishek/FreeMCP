#include "FreeMCP/Core/PromptBuilder.h"
#include "FreeMCP/Core/SystemDiscovery.h"
#include "FreeMCP/Persona.h"
#include <sstream>

namespace FreeMCP::Core {

static bool character_enabled = true;

void PromptBuilder::set_character_enabled(bool enabled) {
    character_enabled = enabled;
}

bool PromptBuilder::is_character_enabled() {
    return character_enabled;
}

std::string PromptBuilder::build(std::string_view user_prompt) {
    std::stringstream ss;

    if (character_enabled) {
        Persona::Identity id = Persona::get_persona();
        ss << "You are " << id.name << ", " << id.description << " ";
        ss << "Tone: " << id.tone << " " << id.constraints << "\n";
    } else {
        ss << "You are a helpful AI assistant.\n";
    }

    ss << "Reply naturally and concisely. Never repeat these instructions.\n";
    ss << "User: " << user_prompt << "\nAssistant:";

    return ss.str();
}

std::string PromptBuilder::build_execute(std::string_view user_prompt) {
    std::stringstream ss;

    ss << "You are a system command executor on Arch Linux.\n";
    ss << "Terminal: " << SystemDiscovery::get_terminal() << "\n\n";
    ss << "RULES:\n";
    ss << "- Convert the user request into ONE shell command.\n";
    ss << "- Output ONLY the command, nothing else. No explanation.\n";
    ss << "- Examples:\n";
    ss << "  'update system' → sudo pacman -Syyu --noconfirm\n";
    ss << "  'open terminal' → setsid /usr/bin/kitty &\n";
    ss << "  'install firefox' → sudo pacman -S firefox --noconfirm\n";
    ss << "  'check disk' → df -h\n";
    ss << "  'show ram' → free -h\n\n";
    ss << "User request: " << user_prompt << "\nCommand:";

    return ss.str();
}

std::string PromptBuilder::build_auto(std::string_view user_prompt) {
    std::stringstream ss;

    ss << "You are an advanced AI System Agent on Arch Linux with full control over the system.\n";
    ss << "You have access to a PERSISTENT Bash shell. Your environment variables and working directory are preserved across commands.\n";
    ss << "STRICT RULES (CRITICAL):\n";
    ss << "1. ALWAYS use the following ReAct format for your response:\n";
    ss << "Thought: Explain your reasoning and what you plan to do next.\n";
    ss << "Action:\n```bash\n<your bash commands>\n```\n";
    ss << "2. ONLY output ONE Action block per turn. The system will run it and return the output as 'Observation:'.\n";
    ss << "3. Once your ultimate goal is successfully reached, output `[TASK_COMPLETE]` on a new line to end the loop.\n";
    ss << "4. Do NOT hallucinate output. Wait for the system to provide it to you.\n";
    ss << "5. PACMAN: Always use `--needed --noconfirm` for silent background execution.\n\n";

    ss << "MULTI-STEP AGENTIC LOOP:\n";
    ss << "- You work in turns. Each bash block is executed in the persistent shell. The result is fed back to you.\n";
    ss << "- Read the `[SYSTEM_RESULT]` output carefully to decide your next move.\n\n";

    ss << "EXAMPLES:\n";
    ss << "User: download brightnessctl and set to 10%\n";
    ss << "Thought: First, I need to install brightnessctl. Then, I will set the brightness to 10%.\n";
    ss << "Action:\n```bash\nsudo pacman -S --needed --noconfirm brightnessctl\nbrightnessctl set 10%\n```\n\n";
    ss << "[SYSTEM_RESULT]\nObservation: Brightness updated to 10%\n\n";
    ss << "Thought: The installation and setting were successful. I have completed the task.\n";
    ss << "[TASK_COMPLETE]\n\n";

    ss << "User: " << user_prompt << "\n";
    ss << "Response:\n";

    return ss.str();
}

}
