// agent_factory.cpp
#include "agent_factory.h"
#include "learning_agent.h"
#include "rule_based_agent.h"

namespace ai_framework {

std::shared_ptr<Agent> AgentFactory::CreateAgent(
    so_5::environment_t& env,
    const std::string& type,
    const std::string& id,
    const std::string& config) {
    
    std::shared_ptr<Agent> agent = nullptr;
    
    env.introduce_coop([&](so_5::coop_t& coop){
        if (type == "learning") {
            coop.make_agent<LearningAgent>(id);
            agent = std::make_shared<LearningAgent>(env, id);
        } else if (type == "rule_based") {
            coop.make_agent<RuleBasedAgent>(id);
            agent = std::make_shared<RuleBasedAgent>(env, id);
        }
    });
    if (agent && !agent->Initialize(config)) {
        agent = nullptr;
    }
    
    return agent;
}

} // namespace ai_framework