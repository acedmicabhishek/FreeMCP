#include "FreeMCP/Core/NetListener.h"
#include <iostream>
#include <string>

void launch_store(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    FreeMCP::Core::NetListener::start(9000);
    
    if (argc > 1 && std::string(argv[1]) == "--store") {
        std::cout << "Launching FreeMCP Store..." << std::endl;
        launch_store(argc, argv);
        return 0;
    }

    std::cout << "FreeMCP: Modular MCP Tool Hub Initialized" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: free-mcp <module> <command> [args]" << std::endl;
        std::cout << "       free-mcp --store" << std::endl;
        return 1;
    }

    std::string moduleName = argv[1];
    std::string command = (argc > 2) ? argv[2] : "";
    std::string args = (argc > 3) ? argv[3] : "";

    std::cout << "Dispatching to module: " << moduleName << "..." << std::endl;
    
    if (moduleName == "archcore") {
        std::cout << "FreeMCP ArchCore Dispatcher Active" << std::endl;
        if (command == "root" || command == "rwx" || command == "terminal") {
            std::cout << "Module " << command << " selected for execution." << std::endl;
            std::cout << "Executing: " << args << std::endl;
        } else {
            std::cerr << "Unknown archcore sub-module: " << command << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Unknown module family: " << moduleName << std::endl;
        return 1;
    }

    return 0;
}
