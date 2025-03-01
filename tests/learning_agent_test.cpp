// learning_agent_test.cpp
#include "catch2/catch.hpp"
#include "../src/learning_agent.h"
#include <so_5/all.hpp>

TEST_CASE("LearningAgent Functionality", "[learning_agent]") {
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    SECTION("Initialize learning agent") {
        const std::string agentId = "test-learning-agent";
        auto agent = std::make_shared<ai_framework::LearningAgent>(env.environment(), agentId);
        
        // Valid configuration
        const std::string validConfig = "{\"learning_rate\": 0.2}";
        REQUIRE(agent->Initialize(validConfig) == true);
        
        // Invalid configuration
        const std::string invalidConfig = "invalid_json";
        REQUIRE(agent->Initialize(invalidConfig) == false);
    }
    
    SECTION("Process messages with learning agent") {
        const std::string agentId = "test-learning-agent-2";
        auto agent = std::make_shared<ai_framework::LearningAgent>(env.environment(), agentId);
        
        // Initialize with memory
        const std::string config = R"({
            "learning_rate": 0.2,
            "initial_memory": {
                "greeting": ["Hello!", "Hi there!"],
                "farewell": ["Goodbye!", "See you later!"]
            }
        })";
        
        REQUIRE(agent->Initialize(config) == true);
        
        // Process a greeting message
        std::string response1 = agent->ProcessMessage("Hello, how are you?");
        REQUIRE(!response1.empty());
        
        // Process a farewell message
        std::string response2 = agent->ProcessMessage("Goodbye for now");
        REQUIRE(!response2.empty());
        
        // Check that responses are different for different message types
        REQUIRE(response1 != response2);
    }
}