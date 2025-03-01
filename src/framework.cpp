// framework.cpp
#include "framework.h"
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <iostream>

namespace ai_framework {

Framework::Framework()
    : m_running(false) {
}

Framework::~Framework() {
    Stop();
}

bool Framework::Initialize(const std::string& config) {
    try {
        // Create and initialize SObjectizer environment
        m_soEnv = std::make_unique<so_5::wrapped_env_t>();
        
        // Create and initialize agent manager
        m_agentManager = std::make_unique<AgentManager>(m_soEnv->environment());
        if (!m_agentManager->Initialize(config)) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing framework: " << e.what() << std::endl;
        return false;
    }
}

bool Framework::Start() {
    if (m_running) {
        return false;
    }
    
    try {
        // Start SObjectizer environment
        m_soEnv->environment().introduce_coop([](so_5::coop_t& coop) {
            // Initialize any global agents here
        });
        
        // Start web server in a separate thread
        m_running = true;
        m_webServerThread = std::make_unique<std::thread>(&Framework::RunWebServer, this, 8080);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error starting framework: " << e.what() << std::endl;
        m_running = false;
        return false;
    }
}

void Framework::Stop() {
    if (!m_running) {
        return;
    }
    
    // Stop the framework
    m_running = false;
    
    // Wait for web server thread to finish
    if (m_webServerThread && m_webServerThread->joinable()) {
        m_webServerThread->join();
    }
    
    // Clear thread object
    m_webServerThread.reset();
    
    // Destroy agent manager
    m_agentManager.reset();
    
    // Shutdown SObjectizer environment
    m_soEnv.reset();
}

AgentManager& Framework::GetAgentManager() {
    return *m_agentManager;
}

void Framework::RunWebServer(int port) {
    using json = nlohmann::json;
    
    // Create uWebSockets app
    uWS::App app;
    
    // Configure routes
    app.get("/agents", [this](auto* res, auto* req) {
        // Get all agent IDs
        auto agentIds = m_agentManager->GetAllAgentIds();
        
        // Create JSON response
        json response = agentIds;
        std::string responseStr = response.dump();
        
        // Send response
        res->writeHeader("Content-Type", "application/json");
        res->end(responseStr);
    });
    
    app.post("/agents", [this](auto* res, auto* req) {
        // Create agent from POST data
        res->onData([this, res](std::string_view data, bool last) {
            if (last) {
                try {
                    // Parse JSON request
                    json request = json::parse(data);
                    
                    // Extract agent details
                    std::string type = request["type"];
                    std::string id = request["id"];
                    std::string config = request["config"].dump();
                    
                    // Create the agent
                    bool success = m_agentManager->CreateAgent(type, id, config);
                    
                    // Create JSON response
                    json response = {
                        {"success", success},
                        {"id", id}
                    };
                    std::string responseStr = response.dump();
                    
                    // Send response
                    res->writeHeader("Content-Type", "application/json");
                    res->end(responseStr);
                } catch (const std::exception& e) {
                    // Handle error
                    json response = {
                        {"success", false},
                        {"error", e.what()}
                    };
                    std::string responseStr = response.dump();
                    
                    res->writeHeader("Content-Type", "application/json");
                    res->writeStatus("400 Bad Request");
                    res->end(responseStr);
                }
            }
        });
    });
    
    app.del("/agents/:id", [this](auto* res, auto* req) {
        // Get agent ID from path parameter
        std::string id = std::string(req->getParameter(0));
        
        // Destroy the agent
        bool success = m_agentManager->DestroyAgent(id);
        
        // Create JSON response
        json response = {
            {"success", success}
        };
        std::string responseStr = response.dump();
        
        // Send response
        res->writeHeader("Content-Type", "application/json");
        res->end(responseStr);
    });
    
    app.post("/agents/:id/message", [this](auto* res, auto* req) {
        // Get agent ID from path parameter
        std::string id = std::string(req->getParameter(0));
        
        // Send message to agent
        res->onData([this, res, id](std::string_view data, bool last) {
            if (last) {
                try {
                    // Parse JSON request
                    json request = json::parse(data);
                    
                    // Extract message
                    std::string message = request["message"];
                    
                    // Send message to agent
                    std::string response = m_agentManager->SendMessage(id, message);
                    
                    // Create JSON response
                    json responseJson = {
                        {"response", response}
                    };
                    std::string responseStr = responseJson.dump();
                    
                    // Send response
                    res->writeHeader("Content-Type", "application/json");
                    res->end(responseStr);
                } catch (const std::exception& e) {
                    // Handle error
                    json response = {
                        {"success", false},
                        {"error", e.what()}
                    };
                    std::string responseStr = response.dump();
                    
                    res->writeHeader("Content-Type", "application/json");
                    res->writeStatus("400 Bad Request");
                    res->end(responseStr);
                }
            }
        });
    });
    
    // Start the server
    app.listen(port, [port](auto* listen_socket) {
        if (listen_socket) {
            std::cout << "Web server listening on port " << port << std::endl;
        } else {
            std::cerr << "Failed to start web server on port " << port << std::endl;
        }
    });
    
    // Run the event loop
    app.run();
}

} // namespace ai_framework