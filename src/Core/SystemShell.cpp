#include "FreeMCP/Core/SystemShell.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <chrono>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Core {

SystemShell::SystemShell() : child_pid(-1) {
    write_pipe[0] = -1; write_pipe[1] = -1;
    read_pipe[0] = -1; read_pipe[1] = -1;
    spawn();
}

SystemShell::~SystemShell() {
    reset();
}

void SystemShell::spawn() {
    signal(SIGPIPE, SIG_IGN);
    if (child_pid > 0) return;

    if (pipe(write_pipe) == -1 || pipe(read_pipe) == -1) {
        log_message("SystemShell: Failed to create pipes.");
        return;
    }

    child_pid = fork();
    if (child_pid == -1) {
        log_message("SystemShell: Failed to fork.");
        return;
    }

    if (child_pid == 0) {
        
        close(write_pipe[1]); 
        close(read_pipe[0]);  

        dup2(write_pipe[0], STDIN_FILENO);
        dup2(read_pipe[1], STDOUT_FILENO);
        dup2(read_pipe[1], STDERR_FILENO);

        
        execl("/bin/bash", "bash", "--noprofile", "--norc", nullptr);
        _exit(1); 
    } else {
        
        close(write_pipe[0]);
        close(read_pipe[1]);
        
        
        int flags = fcntl(read_pipe[0], F_GETFL, 0);
        fcntl(read_pipe[0], F_SETFL, flags | O_NONBLOCK);
    }
}

void SystemShell::reset() {
    std::lock_guard<std::mutex> lock(shell_mutex);
    if (child_pid > 0) {
        kill(child_pid, SIGTERM);
        waitpid(child_pid, nullptr, 0);
        child_pid = -1;
    }
    if (write_pipe[1] != -1) { close(write_pipe[1]); write_pipe[1] = -1; }
    if (read_pipe[0] != -1) { close(read_pipe[0]); read_pipe[0] = -1; }
}

std::string SystemShell::execute_block(std::string_view code, int timeout_ms) {
    std::lock_guard<std::mutex> lock(shell_mutex);
    if (child_pid <= 0) spawn();
    if (child_pid <= 0) return "[SystemShell Error: Cannot spawn shell]";

    
    char dump[4096];
    while (read(read_pipe[0], dump, sizeof(dump)) > 0);

    
    std::string unique_delimeter = "---FREEMCP_CMD_END---";
    std::stringstream ss;
    ss << code << "\n";
    ss << "echo \"" << unique_delimeter << "$?\"\n";
    std::string to_write = ss.str();

    
    ssize_t written = write(write_pipe[1], to_write.data(), to_write.size());
    if (written < 0) {
        log_message("SystemShell: Write error. Respawning shell.");
        reset();
        spawn();
        return "[SystemShell Error: Bad pipe]";
    }

    std::string result;
    auto start_time = std::chrono::steady_clock::now();
    bool finished = false;

    struct pollfd pfd;
    pfd.fd = read_pipe[0];
    pfd.events = POLLIN;

    while (!finished) {
        int ret = poll(&pfd, 1, 100); 
        if (ret > 0) {
            if (pfd.revents & POLLIN) {
                char buffer[1024];
                ssize_t n = read(read_pipe[0], buffer, sizeof(buffer) - 1);
                if (n > 0) {
                    buffer[n] = '\0';
                    result += buffer;
                    
                    size_t delim_pos = result.find(unique_delimeter);
                    if (delim_pos != std::string::npos) {
                        size_t newline_pos = result.find('\n', delim_pos);
                        if (newline_pos == std::string::npos) newline_pos = result.length();
                        result.erase(delim_pos);
                        finished = true;
                    }
                } else if (n == 0) {
                    result += "\n[SystemShell: EOF - Process Died]\n";
                    finished = true;
                    reset();
                }
            }
            if ((pfd.revents & POLLHUP) || (pfd.revents & POLLERR)) {
                if (!finished) {
                    result += "\n[SystemShell: Pipe Broken]\n";
                    finished = true;
                    reset();
                }
            }
        } else if (ret < 0) {
            break; 
        }

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        if (elapsed > timeout_ms) {
            log_message("SystemShell: Command timed out. Sending SIGINT.");
            write(write_pipe[1], "\x03", 1); 
            result += "\n[SystemShell: Command Timeout]\n";
            finished = true;
            
            reset();
            spawn();
        }
    }

    
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) result.pop_back();
    if (result.empty()) result = "[Success (No Output)]";

    return result;
}

} 
