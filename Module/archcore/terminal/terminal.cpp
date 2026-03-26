#include "FreeMCP/Module.h"
#include <iostream>

namespace FreeMCP {

class TerminalModule : public Module {
public:
    std::string getName() const override { return "terminal"; }
    std::vector<std::string> getCapabilities() const override {
        return {"open", "run_command"};
    }
    void execute(const std::string& command, const std::string& args) override {
        std::cout << "TerminalModule: Executing " << command << " command: " << args << std::endl;
        
    }
};

} 
