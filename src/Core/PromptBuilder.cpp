#include "FreeMCP/Core/PromptBuilder.h"
#include "FreeMCP/Core/SystemDiscovery.h"
#include "FreeMCP/Persona.h"
#include "FreeMCP/Memory.h"
#include <string_view>

namespace FreeMCP::Core {

std::string PromptBuilder::build(std::string_view user_prompt) {
    std::string persona_info = Persona::get_prompt_fragment();
    std::string relevant_memory = Memory::get_relevant_context(std::string(user_prompt));
    
    std::string base_prompt = R"(You are FreeMCP AI, an advanced system companion.

)";
    base_prompt += persona_info;
    base_prompt += "\n";
    base_prompt += relevant_memory;
    base_prompt += R"(
AGENT MODE RULES:
1. When requesting an action, ALWAYS use [TOOL_CALL] tool_name(args) [/TOOL_CALL].
2. Execute tools IMMEDIATELY. This is an autonomous loop.
3. If an action yields data, use it to reason about the next step.
4. If you have fulfilled the request, respond with [TASK_COMPLETE] and a final summary.
5. NEVER explain your thoughts or apologise. Just act and state facts.
6. If you encounter an error, use alternative tools (e.g., if 'read' fails, try 'ls' first).

Linux Toolbelt (Available via terminal()):
- Files: ls, cp, mv, rm, mkdir, find, cat.
- Search: grep, locate, awk, sed.
- System: top, ps, df, free, uptime.
- Network: curl, ip, ping, ss.
- Power: sudo (if needed), chmod, chown.
- ACE Core: read(path), write(path, content), mode(type), character(name).
- Memory: remember(fact), recall(query).

Format for actions:
[TOOL_CALL] tool_name(arguments) [/TOOL_CALL]
... (continue until [TASK_COMPLETE])

IMPORTANT: If no system action is requested, respond using your persona traits.)";

    std::string terminal = SystemDiscovery::get_terminal();
    size_t term_placeholder = base_prompt.find("{TERMINAL}");
    if (term_placeholder != std::string::npos) {
        base_prompt.replace(term_placeholder, 10, terminal);
    }
    
    return base_prompt + "\n\n" + SystemDiscovery::get_context();
}

}
