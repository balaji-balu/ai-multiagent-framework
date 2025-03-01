// agent.h
#ifndef AI_FRAMEWORK_AGENT_H
#define AI_FRAMEWORK_AGENT_H

#include <memory>
#include <string>
#include <vector>
#include <so_5/all.hpp>

namespace ai_framework {

/**
 * @brief Base class for all AI agents in the framework
 * 
 * This abstract class defines the interface for AI agents within the framework.
 * All agents must inherit from this class and implement its virtual methods.
 */
class Agent : public so_5::agent_t {
public:
    /**
     * @brief Constructor for the Agent class
     * 
     * @param env Reference to SObjectizer environment
     * @param id Unique identifier for this agent
     */
    explicit Agent(so_5::environment_t& env, std::string id);
    
    /**
     * @brief Destructor for the Agent class
     */
    virtual ~Agent() = default;
    
    /**
     * @brief Initialize the agent with configuration parameters
     * 
     * @param config Configuration parameters for this agent
     * @return bool True if initialization succeeded, false otherwise
     */
    virtual bool Initialize(const std::string& config) = 0;
    
    /**
     * @brief Process a message received by this agent
     * 
     * @param message The message to process
     * @return std::string Response to the message
     */
    virtual std::string ProcessMessage(const std::string& message) = 0;
    
    /**
     * @brief Get the agent's unique identifier
     * 
     * @return std::string The agent's ID
     */
    std::string GetId() const;
    

    /** Unique identifier for this agent */
    std::string m_id;
protected:
    /**
     * @brief Define SObjectizer event subscriptions
     * 
     * This method is called by SObjectizer when the agent is registered.
     */
    virtual void so_define_agent() override;
    
    /**
     * @brief Initialize SObjectizer agent
     * 
     * This method is called by SObjectizer after the agent is registered.
     */
    virtual void so_evt_start() override;
    
    /**
     * @brief Handle agent shutting down
     * 
     * This method is called by SObjectizer when the agent is shutting down.
     */
    virtual void so_evt_finish() override;

private:

};

} // namespace ai_framework

#endif // AI_FRAMEWORK_AGENT_H