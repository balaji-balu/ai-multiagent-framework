// agent_factory_test.cpp
#include "catch2/catch.hpp"
#include "../src/agent_factory.h"
#include "../src/learning_agent.h"
#include "../src/rule_based_agent.h"
#include <so_5/all.hpp>

TEST_CASE("AgentFactory Functionality", "[agent_factory]") {
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    SECTION("Create learning agent") {
        const std::string agentType = "learning";
        const std::string agentId = "test-learning-agent";
        const std::string config = "{\"learning_rate\": 0.2}";
        
        auto agent = ai_framework::AgentFactory::CreateAgent(
            env.environment(), agentType, agentId, config);
        
        REQUIRE(agent != nullptr);
        REQUIRE(agent->GetId() == agentId);
        REQUIRE(dynamic_cast<ai_framework::LearningAgent*>(agent.get()) != nullptr);
    }
    
    SECTION("Create rule-based agent") {
        const std::string agentType = "rule_based";
        const std::string agentId = "test-rule-agent";
        const std::string config = "{\"rules\": [{\"pattern\": \".*hello.*\", \"response\": \"Hi there!\", \"priority\": 10}]}";
        
        auto agent = ai_framework::AgentFactory::CreateAgent(
            env.environment(), agentType, agentId, config);
        
        REQUIRE(agent != nullptr);
        REQUIRE(agent->GetId() == agentId);
        REQUIRE(dynamic_cast<ai_framework::RuleBasedAgent*>(agent.get()) != nullptr);
    }
    
    SECTION("Create unknown agent type") {
        const std::string agentType = "unknown";
        const std::string agentId = "test-unknown-agent";
        const std::string config = "{}";
        
        auto agent = ai_framework::AgentFactory::CreateAgent(
            env.environment(), agentType, agentId, config);
        
        REQUIRE(agent == nullptr);
    }
    
    SECTION("Create agent with invalid config") {
        const std::string agentType = "learning";
        const std::string agentId = "test-invalid-config-agent";
        const std::string config = "invalid_json";
        
        auto agent = ai_framework::AgentFactory::CreateAgent(
            env.environment(), agentType, agentId, config);
        
        REQUIRE(agent == nullptr);
    }
}