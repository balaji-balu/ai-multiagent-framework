// rule_based_agent.h
#ifndef AI_FRAMEWORK_RULE_BASED_AGENT_H
#define AI_FRAMEWORK_RULE_BASED_AGENT_H

#include "agent.h"
#include "messages.h"
#include <map>
#include <regex>
#include <string>
#include <vector>

namespace ai_framework {

/**
 * @brief Structure representing a rule for the rule-based agent
 */
struct Rule {
    /** Pattern to match */
    std::regex pattern;
    
    /** Response template */
    std::string responseTemplate;
    
    /** Priority of the rule (higher values = higher priority) */
    int priority;
};

/**
 * @brief Agent that uses predefined rules to generate responses
 * 
 * This agent matches input messages against predefined patterns
 * and generates responses based on matching rules.
 */
class RuleBasedAgent : public Agent {
public:
    /**
     * @brief Constructor for RuleBasedAgent
     * 
     * @param env Reference to SObjectizer environment
     * @param id Unique identifier for this agent
     */
    RuleBasedAgent(so_5::environment_t& env, std::string id);
    
    /**
     * @brief Destructor for RuleBasedAgent
     */
    virtual ~RuleBasedAgent() = default;
    
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
    /** Rules for this agent */
    std::vector<Rule> m_rules;
    
    /** Default response if no rule matches */
    std::string m_defaultResponse;
    
    /**
     * @brief Add a new rule to the agent
     * 
     * @param pattern Regular expression pattern to match
     * @param responseTemplate Template for the response
     * @param priority Priority of the rule
     */
    void AddRule(const std::string& pattern, const std::string& responseTemplate, int priority);
    
    /**
     * @brief Find the best matching rule for a message
     * 
     * @param message The message to match
     * @return const Rule* Pointer to the matched rule, or nullptr if no match
     */
    const Rule* FindMatchingRule(const std::string& message) const;
    
    /**
     * @brief Generate a response using a rule and a message
     * 
     * @param rule The rule to use
     * @param message The input message
     * @return std::string Generated response
     */
    std::string GenerateRuleResponse(const Rule& rule, const std::string& message) const;

    void HandleMessage(const messages::AgentMessage& msg);
    so_5::mbox_t m_mbox;
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_RULE_BASED_AGENT_H