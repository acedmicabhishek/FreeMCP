#pragma once
#include <string>
#include <string_view>

namespace FreeMCP::Core {

struct IntentResult {
    bool is_action;          
    std::string command;     
    std::string description; 
};

class IntentClassifier {
public:
    static IntentResult classify(std::string_view user_input);
};

}
