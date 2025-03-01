// agent_manager.cpp
#include "agent_manager.h"
#include "agent_factory.h"
#include "rule_based_agent.h"
#include <stdexcept>

namespace ai_framework {

AgentManager::AgentManager(so_5::environment_t& env)
    : m_env(env) {
}

AgentManager::~AgentManager() {
    // Clear all agents
    std::lock_guard<std::mutex> lock(m_agentsMutex);
    m_agents.clear();
}

bool AgentManager::Initialize(const std::string& config) {
    // Implementation would parse config and set up initial state
    return true;
}

bool AgentManager::CreateAgent(
    const std::string& type,
    const std::string& id,
    const std::string& config) {
    
    // Check if an agent with this ID already exists
    {
        std::lock_guard<std::mutex> lock(m_agentsMutex);
        if (m_agents.find(id) != m_agents.end()) {
            return false;
        }
    }
    
    // Create the agent

    auto agent = AgentFactory::CreateAgent(m_env, type, id, config);
    if (!agent) {
        return false;
    }
    
    // // Register the agent with SObjectizer
    
    // // Introduce the agent into the environment
    // ai_framework::Agent* rawAgent = agent.get();
    // m_env.introduce_coop([&](so_5::coop_t& coop){
    //     if (type == "learning") {
    //         LearningAgent
    //         Initialize(config)

    //     } else if (type == "rule_based") {

    //     }
    //     coop.add_agent(agent.get(), ""); //)<RuleBasedAgent>(id)
    // });
   
    // Add the agent to our map
    {
        std::lock_guard<std::mutex> lock(m_agentsMutex);
        m_agents[id] = agent;
    }
    
    return true;
}

bool AgentManager::DestroyAgent(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_agentsMutex);
    
    auto it = m_agents.find(id);
    if (it == m_agents.end()) {
        return false;
    }
    
    // The agent will be deregistered from SObjectizer when the shared_ptr
    // is destroyed
    m_agents.erase(it);
    
    return true;
}

std::string AgentManager::SendMessage(
    const std::string& agentId,
    const std::string& message) {
    
    std::shared_ptr<Agent> agent;
    
    // Get the agent
    {
        std::lock_guard<std::mutex> lock(m_agentsMutex);
        auto it = m_agents.find(agentId);
        if (it == m_agents.end()) {
            throw std::runtime_error("Agent not found: " + agentId);
        }
        agent = it->second;
    }
    
    // Process the message
    return agent->ProcessMessage(message);
}

bool AgentManager::AgentExists(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_agentsMutex);
    return m_agents.find(id) != m_agents.end();
}

void AgentManager::RegisterAgentType(
    const std::string& type, 
    const std::function<std::shared_ptr<Agent>(so_5::environment_t&, const std::string&)>& factory) {
    
    std::lock_guard<std::mutex> lock(m_agentsMutex);
    m_agentFactories[type] = factory;
}

std::vector<std::string> AgentManager::GetAllAgentIds() const {
    std::vector<std::string> ids;
    
    std::lock_guard<std::mutex> lock(m_agentsMutex);
    ids.reserve(m_agents.size());
    
    for (const auto& pair : m_agents) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

} // namespace ai_framework