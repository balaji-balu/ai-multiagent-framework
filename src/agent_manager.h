// agent_manager.h
#ifndef AI_FRAMEWORK_AGENT_MANAGER_H
#define AI_FRAMEWORK_AGENT_MANAGER_H

#include "agent.h"
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <so_5/all.hpp>

namespace ai_framework {

/**
 * @brief Manages the lifecycle of agents in the system
 * 
 * This class keeps track of all active agents, handles agent creation
 * and destruction, and routes messages between agents.
 */
class AgentManager {
public:
    /**
     * @brief Constructor for AgentManager
     * 
     * @param env Reference to SObjectizer environment
     */
    explicit AgentManager(so_5::environment_t& env);
    
    /**
     * @brief Destructor for AgentManager
     */
    ~AgentManager();
    
    /**
     * @brief Initialize the agent manager
     * 
     * @param config Configuration parameters
     * @return bool True if initialization succeeded, false otherwise
     */
    bool Initialize(const std::string& config);
    
    /**
     * @brief Create a new agent in the system
     * 
     * @param type Type of agent to create
     * @param id Unique identifier for the new agent
     * @param config Configuration for the new agent
     * @return bool True if agent was created successfully, false otherwise
     */
    bool CreateAgent(const std::string& type, const std::string& id, const std::string& config);
    
    /**
     * @brief Destroy an existing agent
     * 
     * @param id ID of the agent to destroy
     * @return bool True if agent was destroyed successfully, false otherwise
     */
    bool DestroyAgent(const std::string& id);
    
    /**
     * @brief Send a message to a specific agent
     * 
     * @param agentId ID of the target agent
     * @param message Message to send
     * @return std::string Response from the agent
     */
    std::string SendMessage(const std::string& agentId, const std::string& message);
    
    /**
     * @brief Check if an agent with the given ID exists
     * 
     * @param id Agent ID to check
     * @return bool True if agent exists, false otherwise
     */
    bool AgentExists(const std::string& id) const;
    
    /**
     * @brief Get a list of all agent IDs
     * 
     * @return std::vector<std::string> List of agent IDs
     */
    std::vector<std::string> GetAllAgentIds() const;
    
    static AgentManager& GetInstance(so_5::environment_t& env) {
        static AgentManager instance(env);
        return instance;
    }

    void RegisterAgentType(const std::string& type, 
        const std::function<std::shared_ptr<Agent>(so_5::environment_t &, const std::string &)>& operator_
    );

private:
    /** Reference to SObjectizer environment */
    so_5::environment_t& m_env;
    
    /** Map of agent ID to agent instance */
    std::map<std::string, std::shared_ptr<Agent>> m_agents;
    
    /** Mutex for thread-safe access to the agents map */
    mutable std::mutex m_agentsMutex;

    /** Map of agent type to factory function */
    std::map<std::string, std::function<std::shared_ptr<Agent>(
        so_5::environment_t&, const std::string&)>> m_agentFactories;
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_AGENT_MANAGER_H