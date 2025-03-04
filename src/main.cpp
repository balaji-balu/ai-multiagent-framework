// main.cpp
#include "agent_manager.h"
#include "learning_agent.h"
#include "rule_based_agent.h"
#include "websocket_server.h"
#include "logging_service.h"
#include <so_5/all.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <signal.h>

using namespace ai_framework;

// Global flag for signal handling
volatile sig_atomic_t g_running = 1;

// Signal handler
void signal_handler(int signum) {
    g_running = 0;
}

// Load configuration from file
bool loadConfig(const std::string& filename, nlohmann::json& config) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open configuration file: " << filename << std::endl;
            return false;
        }
        
        file >> config;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading configuration: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    // Process command-line arguments
    std::string configFile = "config.json";
    if (argc > 1) {
        configFile = argv[1];
    }
    
    // Load configuration
    nlohmann::json config;
    if (!loadConfig(configFile, config)) {
        return 1;
    }
    
    // Initialize logging
    std::string logFile = "ai_framework.log";
    LogLevel logLevel = LogLevel::INFO;
    bool logToConsole = true;
    
    if (config.contains("logging")) {
        auto loggingConfig = config["logging"];
        
        if (loggingConfig.contains("file")) {
            logFile = loggingConfig["file"].get<std::string>();
        }
        
        if (loggingConfig.contains("level")) {
            std::string levelStr = loggingConfig["level"].get<std::string>();
            if (levelStr == "TRACE") logLevel = LogLevel::TRACE;
            else if (levelStr == "DEBUG") logLevel = LogLevel::DEBUG;
            else if (levelStr == "INFO") logLevel = LogLevel::INFO;
            else if (levelStr == "WARNING") logLevel = LogLevel::WARNING;
            else if (levelStr == "ERROR") logLevel = LogLevel::ERROR;
            else if (levelStr == "FATAL") logLevel = LogLevel::FATAL;
        }
        
        if (loggingConfig.contains("console")) {
            logToConsole = loggingConfig["console"].get<bool>();
        }
    }
    
    if (!LoggingService::GetInstance().Initialize(logFile, logLevel, logToConsole)) {
        std::cerr << "Failed to initialize logging service" << std::endl;
        return 1;
    }
    
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    // Initialize agent manager
    AgentManager& agentManager = AgentManager::GetInstance(env.environment());
    if (!agentManager.Initialize(config)) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Failed to initialize agent manager");
        return 1;
    }
    
    // Register agent types
    
    agentManager.RegisterAgentType(
        "learning", 
        [](so_5::environment_t& env, const std::string& id) -> std::shared_ptr<Agent> {
            return std::make_shared<LearningAgent>(env, id);
        });
    
    // agentManager.RegisterAgentType(
    //     "rule_based", 
    //     [](so_5::environment_t& env, const std::string& id) -> std::shared_ptr<Agent> {
    //         return std::make_shared<RuleBasedAgent>(env, id);
    //     });
    
    // Create agents based on configuration
    if (config.contains("agents")) {
        for (const auto& agentConfig : config["agents"]) {
            std::string id = agentConfig["id"].get<std::string>();
            std::string type = agentConfig["type"].get<std::string>();
            
            std::string agentConfigStr = agentConfig.dump();
            
            auto agent = agentManager.CreateAgent(type, id, agentConfigStr);
            if (!agent) {
                LoggingService::GetInstance().Log(
                    LogLevel::ERROR, 
                    "Failed to create agent " + id);
            }
        }
    }

    // // Initialize and start the framework
    // Framework framework;
    // if (!framework.Initialize(config.dump())) {
    //     LoggingService::GetInstance().Log(
    //         LogLevel::ERROR, 
    //         "Failed to initialize framework");
    //     return 1;
    // }

    // if (!framework.Start()) {
    //     LoggingService::GetInstance().Log(
    //         LogLevel::ERROR, 
    //         "Failed to start framework");
    //     return 1;
    // }

    // // Initialize WebSocket server
    // WebSocketServer wsServer(config.value("websocket_port", 9090));
    // wsServer.SetMessageHandler([&agentManager](
    //     const std::string& clientId, 
    //     const std::string& message,
    //     std::function<void(const std::string&)> sendResponse) {
        
    //     // Parse the message and route to appropriate agent
    //     // Example implementation
    //     try {
    //         nlohmann::json jsonMessage = nlohmann::json::parse(message);
    //         std::string targetAgent = jsonMessage["agent"].get<std::string>();
    //         std::string content = jsonMessage["message"].get<std::string>();
            
    //         std::string response = agentManager.SendMessage(targetAgent, content);
    //         sendResponse(response);
    //     }
    //     catch (const std::exception& e) {
    //         sendResponse("{\"error\": \"" + std::string(e.what()) + "\"}");
    //     }
    // });

    // if (!wsServer.Start()) {
    //     LoggingService::GetInstance().Log(
    //         LogLevel::ERROR, 
    //         "Failed to start WebSocket server");
    //     return 1;
    // }

    // // Set up signal handling for clean shutdown
    // signal(SIGINT, signal_handler);
    // signal(SIGTERM, signal_handler);

    // // Main loop
    // while (g_running) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    // // Clean shutdown
    // wsServer.Stop();
    // framework.Stop();

}