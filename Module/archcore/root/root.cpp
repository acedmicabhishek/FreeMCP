#include "FreeMCP/Module.h"
#include <iostream>

namespace FreeMCP {

class RootModule : public Module {
public:
    std::string getName() const override { return "root"; }
    std::vector<std::string> getCapabilities() const override {
        return {"elevate", "check_root"};
    }
    void execute(const std::string& command, const std::string& args) override {
        std::cout << "RootModule: Executing " << command << " with args: " << args << std::endl;
        
    }
};

} 
