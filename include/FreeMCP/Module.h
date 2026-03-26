#pragma once
#include <string>
#include <vector>

namespace FreeMCP {

class Module {
public:
    virtual ~Module() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getCapabilities() const = 0;
    virtual void execute(const std::string& command, const std::string& args) = 0;
};

}
