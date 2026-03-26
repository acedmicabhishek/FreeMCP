#include "FreeMCP/Module.h"
#include <iostream>

namespace FreeMCP {

class RWXModule : public Module {
public:
    std::string getName() const override { return "rwx"; }
    std::vector<std::string> getCapabilities() const override {
        return {"read", "write", "execute"};
    }
    void execute(const std::string& command, const std::string& args) override {
        std::cout << "RWXModule: Executing " << command << " on " << args << std::endl;
        
    }
};

} 
