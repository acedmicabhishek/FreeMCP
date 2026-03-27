#pragma once
#include <string>
#include <string_view>
#include <mutex>

namespace FreeMCP::Core {

class SystemShell {
public:
    SystemShell();
    ~SystemShell();

    
    std::string execute_block(std::string_view code, int timeout_ms = 30000);

    
    void reset();

private:
    void spawn();
    
    int child_pid;
    int write_pipe[2];
    int read_pipe[2];
    std::mutex shell_mutex;
};

}
