// agent_manager_test.cpp
#include "catch2/catch.hpp"
#include "../src/agent_manager.h"
#include <so_5/all.hpp>

TEST_CASE("AgentManager Functionality", "[agent_manager]") {
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    // Create agent manager
    ai_framework::AgentManager manager(env.environment());
    
    SECTION("Initialize agent manager") {
        const std::string config = "{\"max_agents\": 100}";
        
        REQUIRE(manager.Initialize(config) == true);
    }
    
    SECTION("Create and destroy agents") {
        // Initialize manager
        REQUIRE(manager.Initialize("{}") == true);
        
        // Create an agent
        const std::string agentType = "learning";
        const std::string agentId = "test-managed-agent";
        const std::string config = "{\"learning_rate\": 0.1}";
        
        REQUIRE(manager.CreateAgent(agentType, agentId, config) == true);
        REQUIRE(manager.AgentExists(agentId) == true);
        
        // Verify the agent is in the list
        auto agentIds = manager.GetAllAgentIds();
        REQUIRE(std::find(agentIds.begin(), agentIds.end(), agentId) != agentIds.end());
        
        // Destroy the agent
        REQUIRE(manager.DestroyAgent(agentId) == true);
        REQUIRE(manager.AgentExists(agentId) == false);
        
        // Verify the agent is no longer in the list
        agentIds = manager.GetAllAgentIds();
        REQUIRE(std::find(agentIds.begin(), agentIds.end(), agentId) == agentIds.end());
    }
    
    SECTION("Create agent with duplicate ID") {
        // Initialize manager
        REQUIRE(manager.Initialize("{}") == true);
        
        // Create an agent
        const std::string agentType = "learning";
        const std::string agentId = "test-duplicate-agent";
        const std::string config = "{\"learning_rate\": 0.1}";
        
        REQUIRE(manager.CreateAgent(agentType, agentId, config) == true);
        
        // Try to create another agent with the same ID
        REQUIRE(manager.CreateAgent(agentType, agentId, config) == false);
        
        // Clean up
        REQUIRE(manager.DestroyAgent(agentId) == true);
    }
    
    SECTION("Destroy non-existent agent") {
        // Initialize manager
        REQUIRE(manager.Initialize("{}") == true);
        
        // Try to destroy an agent that doesn't exist
        const std::string agentId = "non-existent-agent";
        
        REQUIRE(manager.DestroyAgent(agentId) == false);
    }
    
    SECTION("Send message to agent") {
        // Initialize manager
        REQUIRE(manager.Initialize("{}") == true);
        
        // Create an agent
        const std::string agentType = "rule_based";
        const std::string agentId = "test-message-agent";
        const std::string config = "{\"rules\": [{\"pattern\": \".*hello.*\", \"response\": \"Hi there!\", \"priority\": 10}]}";
        
        REQUIRE(manager.CreateAgent(agentType, agentId, config) == true);
        
        // Send a message to the agent
        const std::string message = "hello world";
        std::string response;
        
        REQUIRE_NOTHROW(response = manager.SendMessage(agentId, message));
        REQUIRE(response == "Hi there!");
        
        // Clean up
        REQUIRE(manager.DestroyAgent(agentId) == true);
    }
    
    SECTION("Send message to non-existent agent") {
        // Initialize manager
        REQUIRE(manager.Initialize("{}") == true);
        
        // Try to send a message to an agent that doesn't exist
        const std::string agentId = "non-existent-agent";
        const std::string message = "hello";
        
        REQUIRE_THROWS_AS(manager.SendMessage(agentId, message), std::runtime_error);
    }
}