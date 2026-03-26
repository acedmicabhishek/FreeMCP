#include "FreeMCP/Memory.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>

namespace Memory {

static const char* MEMORY_FILE = ".freemcp_memory";

struct MemoryEntry {
    std::string timestamp;
    std::string content;
    int score = 0;
};

static std::string get_current_time() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return buf;
}

void store(const std::string& fact) {
    if (fact.empty()) return;
    
    std::ifstream in(MEMORY_FILE);
    std::string line;
    while (std::getline(in, line)) {
        if (line.find(fact) != std::string::npos) return;
    }
    in.close();

    std::ofstream out(MEMORY_FILE, std::ios::app);
    if (out.is_open()) {
        out << get_current_time() << " | " << fact << "\n";
    }
}

std::vector<std::string> recall(const std::string& query) {
    std::vector<std::string> results;
    std::ifstream in(MEMORY_FILE);
    if (!in.is_open()) return results;

    std::string line;
    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);

    while (std::getline(in, line)) {
        std::string l = line;
        std::transform(l.begin(), l.end(), l.begin(), ::tolower);
        if (l.find(q) != std::string::npos) {
            results.push_back(line);
        }
    }
    return results;
}

std::string get_relevant_context(const std::string& user_prompt) {
    std::ifstream in(MEMORY_FILE);
    if (!in.is_open()) return "";

    std::vector<MemoryEntry> entries;
    std::string line;
    while (std::getline(in, line)) {
        size_t pipe = line.find(" | ");
        if (pipe != std::string::npos) {
            entries.push_back({line.substr(0, pipe), line.substr(pipe + 3), 0});
        } else {
            entries.push_back({"", line, 0});
        }
    }

    std::string p = user_prompt;
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);
    std::stringstream ps(p);
    std::string word;
    std::vector<std::string> keywords;
    while (ps >> word) if (word.length() > 3) keywords.push_back(word);

    for (size_t i = 0; i < entries.size(); ++i) {
        if (i > entries.size() - 10) entries[i].score += 5;
        
        std::string content_lower = entries[i].content;
        std::transform(content_lower.begin(), content_lower.end(), content_lower.begin(), ::tolower);
        for (const auto& kw : keywords) {
            if (content_lower.find(kw) != std::string::npos) entries[i].score += 10;
        }

        if (entries[i].content.find("!") == 0 || entries[i].content.find("IMPORTANT") != std::string::npos) {
            entries[i].score += 15;
        }
    }

    std::sort(entries.begin(), entries.end(), [](const MemoryEntry& a, const MemoryEntry& b) {
        return a.score > b.score;
    });

    std::stringstream ss;
    ss << "LONG-TERM MEMORY (RELEVANT CONTEXT):\n";
    int count = 0;
    for (const auto& entry : entries) {
        if (count >= 15 || entry.score <= 0) break;
        ss << "[" << entry.timestamp << "] " << entry.content << "\n";
        count++;
    }
    
    if (count == 0 && !entries.empty()) {
        for (int i = std::max(0, (int)entries.size() - 5); i < (int)entries.size(); i++) {
             ss << "[" << entries[i].timestamp << "] " << entries[i].content << "\n";
        }
    }

    return ss.str();
}

void clear() {
    std::ofstream out(MEMORY_FILE, std::ios::trunc);
}

}
