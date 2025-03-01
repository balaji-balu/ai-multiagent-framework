// learning_agent.h
#ifndef AI_FRAMEWORK_LEARNING_AGENT_H
#define AI_FRAMEWORK_LEARNING_AGENT_H

#include "agent.h"
#include "messages.h"
#include <map>
#include <string>
#include <vector>

namespace ai_framework {

/**
 * @brief Agent that learns from interactions
 * 
 * This agent implements a simple learning algorithm that improves
 * its responses based on past interactions.
 */
class LearningAgent : public Agent {
public:
    /**
     * @brief Constructor for LearningAgent
     * 
     * @param env Reference to SObjectizer environment
     * @param id Unique identifier for this agent
     */
    LearningAgent(so_5::environment_t& env, std::string id);
    
    /**
     * @brief Destructor for LearningAgent
     */
    virtual ~LearningAgent() = default;
    
    /**
     * @brief Initialize the agent with configuration parameters
     * 
     * @param config Configuration parameters for this agent
     * @return bool True if initialization succeeded, false otherwise
     */
    virtual bool Initialize(const std::string& config) override;
    
    /**
     * @brief Process a message received by this agent
     * 
     * @param message The message to process
     * @return std::string Response to the message
     */
    virtual std::string ProcessMessage(const std::string& message) override;

protected:
    /**
     * @brief Define SObjectizer event subscriptions
     */
    virtual void so_define_agent() override;
    
    /**
     * @brief Initialize SObjectizer agent
     */
    virtual void so_evt_start() override;
    
    /**
     * @brief Handle agent shutting down
     */
    virtual void so_evt_finish() override;

private:
    /** Learning rate parameter */
    double m_learningRate;
    
    /** Memory of past interactions */
    std::map<std::string, std::vector<std::string>> m_memory;
    
    /**
     * @brief Extract features from a message
     * 
     * @param message The message to analyze
     * @return std::vector<std::string> Features extracted from the message
     */
    std::vector<std::string> ExtractFeatures(const std::string& message);
    
    /**
     * @brief Generate a response based on current knowledge
     * 
     * @param features Features of the input message
     * @return std::string Generated response
     */
    std::string GenerateResponse(const std::vector<std::string>& features);
    
    /**
     * @brief Update the agent's knowledge based on an interaction
     * 
     * @param message The input message
     * @param response The generated response
     */
    void UpdateKnowledge(const std::string& message, const std::string& response);

    bool LoadMemory();
    bool SaveMemory();
    void HandleMessage(const messages::AgentMessage& msg);
    std::mutex m_memoryMutex;
    so_5::mbox_t m_mbox;
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_LEARNING_AGENT_H