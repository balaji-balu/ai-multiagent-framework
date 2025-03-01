// framework_test.cpp
#include "catch2/catch.hpp"
#include "../src/framework.h"
#include <chrono>
#include <thread>

TEST_CASE("Framework Functionality", "[framework]") {
    SECTION("Initialize framework") {
        ai_framework::Framework framework;
        const std::string config = "{\"port\": 8081}";
        
        REQUIRE(framework.Initialize(config) == true);
    }
    
    SECTION("Start and stop framework") {
        ai_framework::Framework framework;
        
        REQUIRE(framework.Initialize("{}") == true);
        REQUIRE(framework.Start() == true);
        
        // Wait a short time to ensure the framework is running
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Stop the framework
        framework.Stop();
    }
    
    SECTION("Get agent manager") {
        ai_framework::Framework framework;
        
        REQUIRE(framework.Initialize("{}") == true);
        
        // Get the agent manager
        ai_framework::AgentManager& manager = framework.GetAgentManager();
        
        // Verify that the manager works
        REQUIRE(manager.Initialize("{}") == true);
    }
}