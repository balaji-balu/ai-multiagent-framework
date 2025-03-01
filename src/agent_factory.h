// agent_factory.h
#ifndef AI_FRAMEWORK_AGENT_FACTORY_H
#define AI_FRAMEWORK_AGENT_FACTORY_H

#include "agent.h"
#include <memory>
#include <string>
#include <so_5/all.hpp>

namespace ai_framework {

/**
 * @brief Factory class for creating agent instances
 * 
 * This class is responsible for creating different types of agents
 * based on type identifiers.
 */
class AgentFactory {
public:
    /**
     * @brief Create an agent of the specified type
     * 
     * @param env Reference to SObjectizer environment
     * @param type The type of agent to create
     * @param id Unique identifier for the new agent
     * @param config Configuration for the new agent
     * @return std::shared_ptr<Agent> Pointer to the created agent
     */
    static std::shared_ptr<Agent> CreateAgent(
        so_5::environment_t& env,
        const std::string& type,
        const std::string& id,
        const std::string& config);
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_AGENT_FACTORY_H