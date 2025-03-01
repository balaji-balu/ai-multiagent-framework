// rule_based_agent_test.cpp
#include "catch2/catch.hpp"
#include "../src/rule_based_agent.h"
#include <so_5/all.hpp>

TEST_CASE("RuleBasedAgent Functionality", "[rule_based_agent]") {
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    SECTION("Initialize rule-based agent") {
        const std::string agentId = "test-rule-agent";
        auto agent = std::make_shared<ai_framework::RuleBasedAgent>(env.environment(), agentId);
        
        // Valid configuration
        const std::string validConfig = R"({
            "rules": [
                {"pattern": ".*hello.*", "response": "Hi there!", "priority": 10},
                {"pattern": ".*bye.*", "response": "Goodbye!", "priority": 5}
            ],
            "default_response": "I don't understand."
        })";
        
        REQUIRE(agent->Initialize(validConfig) == true);
        
        // Invalid configuration
        const std::string invalidConfig = "invalid_json";
        REQUIRE(agent->Initialize(invalidConfig) == false);
    }
    
    SECTION("Process messages with rule-based agent") {
        const std::string agentId = "test-rule-agent-2";
        auto agent = std::make_shared<ai_framework::RuleBasedAgent>(env.environment(), agentId);
        
        // Initialize with rules
        const std::string config = R"({
            "rules": [
                {"pattern": ".*hello.*", "response": "Hi there!", "priority": 10},
                {"pattern": ".*bye.*", "response": "Goodbye!", "priority": 5},
                {"pattern": ".*name.*", "response": "My name is ${agent_id}.", "priority": 8}
            ],
            "default_response": "I don't understand."
        })";
        
        REQUIRE(agent->Initialize(config) == true);
        
        // Test matching first rule
        REQUIRE(agent->ProcessMessage("hello world") == "Hi there!");
        
        // Test matching second rule
        REQUIRE(agent->ProcessMessage("saying bye now") == "Goodbye!");
        
        // Test matching third rule with template substitution
        REQUIRE(agent->ProcessMessage("what is your name?") == "My name is test-rule-agent-2.");
        
        // Test default response
        REQUIRE(agent->ProcessMessage("something random") == "I don't understand.");
    }
}