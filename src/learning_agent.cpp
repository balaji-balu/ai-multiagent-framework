// learning_agent.cpp
#include "learning_agent.h"
#include "logging_service.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <regex>

namespace ai_framework {

LearningAgent::LearningAgent(so_5::environment_t& env, std::string id)
    : Agent(env, std::move(id)), m_learningRate(0.1), m_mbox(so_direct_mbox()) {
}

bool LearningAgent::Initialize(const std::string& config) {
    try {
        // Parse configuration JSON
        nlohmann::json configJson = nlohmann::json::parse(config);
        
        // Extract learning rate if provided
        if (configJson.contains("learning_rate")) {
            m_learningRate = configJson["learning_rate"].get<double>();
        }
        
        // Initialize memory if provided
        if (configJson.contains("initial_memory")) {
            std::lock_guard<std::mutex> lock(m_memoryMutex);
            auto memoryJson = configJson["initial_memory"];
            for (auto it = memoryJson.begin(); it != memoryJson.end(); ++it) {
                std::string key = it.key();
                std::vector<std::string> responses;
                
                for (const auto& value : it.value()) {
                    responses.push_back(value.get<std::string>());
                }
                
                m_memory[key] = responses;
            }
        }
        else {
            // Try to load memory from persistent storage
            LoadMemory();
        }
        
        LoggingService::GetInstance().Log(
            LogLevel::INFO, 
            "LearningAgent " + m_id + " initialized with learning rate " + 
            std::to_string(m_learningRate));
        
        return true;
    } 
    catch (const std::exception& e) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Failed to initialize LearningAgent " + m_id + ": " + e.what());
        return false;
    }
}

std::string LearningAgent::ProcessMessage(const std::string& message) {
    // Extract features from the message
    auto features = ExtractFeatures(message);
    
    // Generate a response based on the features
    std::string response = GenerateResponse(features);
    
    // Update the agent's knowledge
    UpdateKnowledge(message, response);
    
    LoggingService::GetInstance().Log(
        LogLevel::DEBUG, 
        "LearningAgent " + m_id + " processed message and generated response");
    
    return response;
}

void LearningAgent::so_define_agent() {
    // Subscribe to agent messages
    so_subscribe(m_mbox).event([this](const messages::AgentMessage& msg) {
        this->HandleMessage(msg);
    });
}

void LearningAgent::so_evt_start() {
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "LearningAgent " + m_id + " started");
}

void LearningAgent::so_evt_finish() {
    // Save memory before shutting down
    SaveMemory();
    
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "LearningAgent " + m_id + " finished");
}

std::vector<std::string> LearningAgent::ExtractFeatures(const std::string& message) {
    std::vector<std::string> features;
    
    // Simple feature extraction: tokenize by spaces and remove punctuation
    std::string normalized = message;
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](char c) { return static_cast<char>(std::tolower(c)); });
    
    // Remove punctuation
    normalized.erase(
        std::remove_if(normalized.begin(), normalized.end(),
                      [](char c) { return std::ispunct(static_cast<unsigned char>(c)); }),
        normalized.end());
    
    // Split by spaces
    std::istringstream iss(normalized);
    std::string token;
    while (std::getline(iss, token, ' ')) {
        if (!token.empty()) {
            features.push_back(token);
        }
    }
    
    return features;
}

std::string LearningAgent::GenerateResponse(const std::vector<std::string>& features) {
    if (features.empty()) {
        return "I don't understand your message.";
    }
    
    // Create a key from the features
    std::string key = features.front();
    for (size_t i = 1; i < std::min(features.size(), static_cast<size_t>(3)); ++i) {
        key += "_" + features[i];
    }
    
    std::lock_guard<std::mutex> lock(m_memoryMutex);
    
    // Check if we have responses for this key
    auto it = m_memory.find(key);
    if (it != m_memory.end() && !it->second.empty()) {
        // Return a random response from our memory
        size_t index = static_cast<size_t>(
            std::rand() % static_cast<int>(it->second.size()));
        return it->second[index];
    }
    
    // Fallback to a default response
    return "I'm still learning how to respond to that.";
}

void LearningAgent::UpdateKnowledge(const std::string& message, const std::string& response) {
    auto features = ExtractFeatures(message);
    if (features.empty()) {
        return;
    }
    
    // Create a key from the features
    std::string key = features.front();
    for (size_t i = 1; i < std::min(features.size(), static_cast<size_t>(3)); ++i) {
        key += "_" + features[i];
    }
    
    std::lock_guard<std::mutex> lock(m_memoryMutex);
    
    // Add the response to memory
    m_memory[key].push_back(response);
    
    // Limit the number of responses per key to prevent memory explosion
    constexpr size_t MAX_RESPONSES = 10;
    if (m_memory[key].size() > MAX_RESPONSES) {
        m_memory[key].erase(m_memory[key].begin());
    }
}

bool LearningAgent::SaveMemory()  {
    try {
        std::lock_guard<std::mutex> lock(m_memoryMutex);
        
        // Convert memory to JSON
        nlohmann::json memoryJson = nlohmann::json::object();
        for (const auto& pair : m_memory) {
            memoryJson[pair.first] = pair.second;
        }
        
        // Save to file
        std::string filename = "memory_" + m_id + ".json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            LoggingService::GetInstance().Log(
                LogLevel::ERROR, 
                "Failed to open memory file for writing: " + filename);
            return false;
        }
        
        file << memoryJson.dump(4);
        return true;
    }
    catch (const std::exception& e) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Failed to save memory: " + std::string(e.what()));
        return false;
    }
}

bool LearningAgent::LoadMemory() {
    try {
        std::string filename = "memory_" + m_id + ".json";
        std::ifstream file(filename);
        if (!file.is_open()) {
            LoggingService::GetInstance().Log(
                LogLevel::WARNING, 
                "Failed to open memory file for reading: " + filename);
            return false;
        }
        
        nlohmann::json memoryJson;
        file >> memoryJson;
        
        std::lock_guard<std::mutex> lock(m_memoryMutex);
        for (auto it = memoryJson.begin(); it != memoryJson.end(); ++it) {
            std::string key = it.key();
            std::vector<std::string> responses;
            
            for (const auto& value : it.value()) {
                responses.push_back(value.get<std::string>());
            }
            
            m_memory[key] = responses;
        }
        
        LoggingService::GetInstance().Log(
            LogLevel::INFO, 
            "Loaded memory for agent " + m_id + " with " + 
            std::to_string(m_memory.size()) + " entries");
        
        return true;
    }
    catch (const std::exception& e) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Failed to load memory: " + std::string(e.what()));
        return false;
    }
}

void LearningAgent::HandleMessage(const messages::AgentMessage& msg){

}

} // namespace ai_framework