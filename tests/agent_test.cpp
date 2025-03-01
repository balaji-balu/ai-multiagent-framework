// agent_test.cpp
#include "catch2/catch.hpp"
#include "../src/agent.h"
#include "../src/messages.h"
#include <so_5/all.hpp>

// Mock agent implementation for testing
class MockAgent : public ai_framework::Agent {
public:
    MockAgent(so_5::environment_t& env, std::string id)
        : Agent(env, std::move(id)), m_initializeSucceeds(true), m_initialized(false) {}
    
    void SetInitializeSucceeds(bool succeeds) {
        m_initializeSucceeds = succeeds;
    }
    
    bool IsInitialized() const {
        return m_initialized;
    }
    
    void SetProcessMessageResponse(const std::string& response) {
        m_processMessageResponse = response;
    }
    
    virtual bool Initialize(const std::string& config) override {
        m_lastConfig = config;
        m_initialized = m_initializeSucceeds;
        return m_initialized;
    }
    
    virtual std::string ProcessMessage(const std::string& message) override {
        m_lastMessage = message;
        return m_processMessageResponse;
    }
    
    std::string GetLastMessage() const {
        return m_lastMessage;
    }
    
    std::string GetLastConfig() const {
        return m_lastConfig;
    }
    
protected:
    virtual void so_define_agent() override {
        // Define event handlers
    }
    
    virtual void so_evt_start() override {
        // Nothing to do
    }
    
    virtual void so_evt_finish() override {
        // Nothing to do
    }
    
private:
    bool m_initializeSucceeds;
    bool m_initialized;
    std::string m_lastConfig;
    std::string m_lastMessage;
    std::string m_processMessageResponse;
};

TEST_CASE("Agent Basic Functionality", "[agent]") {
    // Create SObjectizer environment
    so_5::wrapped_env_t env;
    
    SECTION("Agent ID is set correctly") {
        const std::string agentId = "test-agent-1";
        auto agent = std::make_shared<MockAgent>(env.environment(), agentId);
        
        REQUIRE(agent->GetId() == agentId);
    }
    
    SECTION("Agent initialization succeeds") {
        const std::string agentId = "test-agent-2";
        const std::string config = "{\"key\": \"value\"}";
        auto agent = std::make_shared<MockAgent>(env.environment(), agentId);
        
        agent->SetInitializeSucceeds(true);
        
        REQUIRE(agent->Initialize(config) == true);
        REQUIRE(agent->IsInitialized() == true);
        REQUIRE(agent->GetLastConfig() == config);
    }
    
    SECTION("Agent initialization fails") {
        const std::string agentId = "test-agent-3";
        const std::string config = "{\"key\": \"value\"}";
        auto agent = std::make_shared<MockAgent>(env.environment(), agentId);
        
        agent->SetInitializeSucceeds(false);
        
        REQUIRE(agent->Initialize(config) == false);
        REQUIRE(agent->IsInitialized() == false);
        REQUIRE(agent->GetLastConfig() == config);
    }
    
    SECTION("Agent processes messages correctly") {
        const std::string agentId = "test-agent-4";
        const std::string message = "Hello, agent!";
        const std::string expectedResponse = "Hello, human!";
        auto agent = std::make_shared<MockAgent>(env.environment(), agentId);
        
        agent->SetProcessMessageResponse(expectedResponse);
        
        REQUIRE(agent->ProcessMessage(message) == expectedResponse);
        REQUIRE(agent->GetLastMessage() == message);
    }
}