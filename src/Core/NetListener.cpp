#include "FreeMCP/Core/NetListener.h"
#include "FreeMCP/Core/ChatEngine.h"
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

extern "C" void log_message(const char* msg);

#include "FreeMCP/Core/ConfigManager.h"
#include "FreeMCP/Core/PromptBuilder.h"
#include "FreeMCP/UI/StoreApp.h"
#include "FreeMCP/UI/Views/ChatView.h"
#include "FreeMCP/Core/Utils.h"
#include "FreeMCP/Core/ToolExecutor.h"

namespace FreeMCP::Core {

bool NetListener::running = false;

struct ChatSyncData {
    std::string sender;
    std::string message;
};

static gboolean sync_chat_cb(gpointer user_data) {
    ChatSyncData* data = (ChatSyncData*)user_data;
    if (global_app && global_app->chat_history) {
        FreeMCP::UI::Views::append_to_chat(global_app, data->sender.c_str(), data->message.c_str());
    }
    delete data;
    return FALSE;
}

static void sync_chat(const std::string& sender, const std::string& message) {
    ChatSyncData* data = new ChatSyncData{sender, message};
    g_idle_add(sync_chat_cb, data);
}

void NetListener::start(int port) {
    if (running) return;
    running = true;
    std::thread(listen_loop, port).detach();
}

void NetListener::stop() {
    running = false;
}

static void handle_client(int client_socket) {
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    char buffer[1024] = {0};
    int valread = read(client_socket, buffer, 1024);
    if (valread > 0) {
        std::string prompt(buffer, valread);

        
        if (prompt == "PING") {
            std::string resp = "PONG";
            send(client_socket, resp.c_str(), resp.length(), 0);
            close(client_socket);
            return;
        }

        log_message(("REMOTE PROMPT RECEIVED: " + prompt).c_str());

        
        bool is_system_cmd = (prompt == "LAUNCH_FREEMCP"
                           || prompt.find("SET_KILL_SWITCH ") == 0
                           || prompt.find("SET_CHARACTER_MODE ") == 0
                           || prompt.find("SET_BRAIN ") == 0);
        if (!is_system_cmd) {
            sync_chat("Remote (Phone)", prompt);
        }
        
        std::string lower_prompt = Utils::to_lower(prompt);
        if (lower_prompt.find("do :") == 0) {
            std::string action = prompt.substr(4);
            while (!action.empty() && action.front() == ' ') action.erase(action.begin());
            log_message(("DIRECT EXECUTION: " + action).c_str());
            std::string output = ToolExecutor::execute(action);
            send(client_socket, output.c_str(), output.length(), 0);
            sync_chat("System", output);
        } else if (prompt == "LAUNCH_FREEMCP") {
            log_message("EXECUTING REMOTE LAUNCH: FreeMCP GUI");
            system("./FreeMCP &");
            std::string resp = "FreeMCP Dashboard Launching...";
            send(client_socket, resp.c_str(), resp.length(), 0);
            sync_chat("System", resp);
        } else if (prompt.find("SET_KILL_SWITCH ") == 0) {
            std::string state = prompt.substr(16);
            ToolExecutor::set_kill_switch(state == "ON");
            log_message(("REMOTE CMD: Kill Switch set to " + state).c_str());
            std::string resp = "Kill Switch set to: " + state;
            send(client_socket, resp.c_str(), resp.length(), 0);
            sync_chat("System", resp);
        } else if (prompt.find("SET_CHARACTER_MODE ") == 0) {
            std::string state = prompt.substr(19);
            PromptBuilder::set_character_enabled(state == "ON");
            log_message(("REMOTE CMD: Character Mode set to " + state).c_str());
            std::string resp = "Character Mode: " + state;
            send(client_socket, resp.c_str(), resp.length(), 0);
        } else if (prompt.find("SET_BRAIN ") == 0) {
            std::string new_brain = prompt.substr(10);
            ConfigManager::save("brain", new_brain);
            log_message(("REMOTE CONFIG: Brain set to " + new_brain).c_str());
            std::string resp = "Brain updated to: " + new_brain;
            send(client_socket, resp.c_str(), resp.length(), 0);
            sync_chat("System", resp);
        } else {
            std::string actual_prompt = prompt;
            
            if (prompt.find("EXEC:") == 0 || prompt.find("AUTO:") == 0) {
                actual_prompt = prompt.substr(5);
            }
            while (!actual_prompt.empty() && actual_prompt.front() == ' ') actual_prompt.erase(actual_prompt.begin());

            
            
            
            log_message(("AUTO AI: " + actual_prompt).c_str());
            std::string response = ChatEngine::chat_auto("", actual_prompt);
            send(client_socket, response.c_str(), response.length(), 0);
            sync_chat("System", response);
        }
    }
    close(client_socket);
}



void NetListener::listen_loop(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) return;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) return;
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) return;
    if (listen(server_fd, 3) < 0) return;
    
    log_message(("Remote Listener started on port " + std::to_string(port)).c_str());
    
    while (running) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            if (!running) break;
            continue;
        }
        std::thread(handle_client, new_socket).detach();
    }
    close(server_fd);
}

}
