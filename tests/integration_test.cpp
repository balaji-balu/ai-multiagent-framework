// integration_test.cpp
#include "catch2/catch.hpp"
#include "../src/framework.h"
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <string>

// Helper function to write CURL response data to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), newLength);
        return newLength;
    } catch (std::bad_alloc& e) {
        return 0;
    }
}

TEST_CASE("Integration Tests", "[integration]") {
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Start the framework
    ai_framework::Framework framework;
    REQUIRE(framework.Initialize("{\"port\": 8082}") == true);
    REQUIRE(framework.Start() == true);
    
    // Wait for the server to start
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    SECTION("Create and use agents via REST API") {
        CURL* curl = curl_easy_init();
        REQUIRE(curl != nullptr);
        
        if (curl) {
            // Create a new agent
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            
            std::string createAgentData = R"({
                "type": "rule_based",
                "id": "test-rest-agent",
                "config": {
                    "rules": [
                        {"pattern": ".*hello.*", "response": "Hi from REST API!", "priority": 10}
                    ],
                    "default_response": "I don't understand."
                }
            })";
            
            std::string createResponse;
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8082/agents");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, createAgentData.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &createResponse);
            
            CURLcode res = curl_easy_perform(curl);
            REQUIRE(res == CURLE_OK);
            
            // Send a message to the agent
            std::string messageData = R"({"message": "hello from test"})";
            std::string messageResponse;
            
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8082/agents/test-rest-agent/message");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, messageData.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &messageResponse);
            
            res = curl_easy_perform(curl);
            REQUIRE(res == CURLE_OK);
            REQUIRE(messageResponse.find("Hi from REST API!") != std::string::npos);
            
            // Get list of agents
            std::string listResponse;
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8082/agents");
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &listResponse);
            
            res = curl_easy_perform(curl);
            REQUIRE(res == CURLE_OK);
            REQUIRE(listResponse.find("test-rest-agent") != std::string::npos);
            
            // Delete the agent
            std::string deleteResponse;
            curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8082/agents/test-rest-agent");
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &deleteResponse);
            
            res = curl_easy_perform(curl);
            REQUIRE(res == CURLE_OK);
            
            // Clean up
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
    }
    
    // Stop the framework
    framework.Stop();
    
    // Clean up CURL
    curl_global_cleanup();
}