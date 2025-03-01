// framework.h
#ifndef AI_FRAMEWORK_FRAMEWORK_H
#define AI_FRAMEWORK_FRAMEWORK_H

#include "agent_manager.h"
#include <string>
#include <memory>
#include <thread>
#include <so_5/all.hpp>

namespace ai_framework {

/**
 * @brief Main class for the AI Framework
 * 
 * This class initializes and manages the AI framework, including
 * the SObjectizer environment and the web server.
 */
class Framework {
public:
    /**
     * @brief Constructor for Framework
     */
    Framework();
    
    /**
     * @brief Destructor for Framework
     */
    ~Framework();
    
    /**
     * @brief Initialize the framework
     * 
     * @param config Configuration parameters
     * @return bool True if initialization succeeded, false otherwise
     */
    bool Initialize(const std::string& config);
    
    /**
     * @brief Start the framework
     * 
     * @return bool True if started successfully, false otherwise
     */
    bool Start();
    
    /**
     * @brief Stop the framework
     */
    void Stop();
    
    /**
     * @brief Get reference to the agent manager
     * 
     * @return AgentManager& Reference to the agent manager
     */
    AgentManager& GetAgentManager();

private:
    /** SObjectizer environment */
    std::unique_ptr<so_5::wrapped_env_t> m_soEnv;
    
    /** Agent manager */
    std::unique_ptr<AgentManager> m_agentManager;
    
    /** Thread for the web server */
    std::unique_ptr<std::thread> m_webServerThread;
    
    /** Flag indicating if the framework is running */
    bool m_running;
    
    /**
     * @brief Run the web server
     * 
     * @param port Port to listen on
     */
    void RunWebServer(int port);
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_FRAMEWORK_H