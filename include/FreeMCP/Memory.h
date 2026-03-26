#ifndef FREEMCP_MEMORY_H
#define FREEMCP_MEMORY_H

#include <string>
#include <vector>

namespace Memory {
    void store(const std::string& fact);
    std::vector<std::string> recall(const std::string& query);
    std::string get_relevant_context(const std::string& user_prompt);
    void clear();
}

#endif
