#ifndef FREEMCP_OLLAMA_H
#define FREEMCP_OLLAMA_H

#include <vector>
#include <string>
#include "FreeMCP/Core/SystemDiscovery.h"
#include "FreeMCP/Core/OllamaClient.h"
#include "FreeMCP/Core/ChatEngine.h"

namespace Ollama {
    inline void discover_system() { FreeMCP::Core::SystemDiscovery::discover(); }
    inline std::vector<std::string> list_models() { return FreeMCP::Core::OllamaClient::list_models(); }
    inline bool pull_model(const std::string& name) { return FreeMCP::Core::OllamaClient::pull_model(name); }
    inline bool remove_model(const std::string& name) { return FreeMCP::Core::OllamaClient::remove_model(name); }
    inline bool ensure_service_active() { return FreeMCP::Core::OllamaClient::ensure_service_active(); }
    inline bool run_model(const std::string& name) { return FreeMCP::Core::OllamaClient::run_model(name); }
    inline std::string chat(const std::string& model, const std::string& prompt) { return FreeMCP::Core::ChatEngine::chat(model, prompt); }
}

#endif
