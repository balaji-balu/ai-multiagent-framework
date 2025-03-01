// rule_based_agent.cpp
#include "rule_based_agent.h"
#include "logging_service.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>

namespace ai_framework {

RuleBasedAgent::RuleBasedAgent(so_5::environment_t& env, std::string id)
    : Agent(env, std::move(id)), 
      m_defaultResponse("I don't have a specific rule for that."), 
      m_mbox(so_direct_mbox()) {
}

bool RuleBasedAgent::Initialize(const std::string& config) {
    try {
        // Parse configuration JSON
        nlohmann::json configJson = nlohmann::json::parse(config);
        
        // Extract default response if provided
        if (configJson.contains("default_response")) {
            m_defaultResponse = configJson["default_response"].get<std::string>();
        }
        
        // Extract rules if provided
        if (configJson.contains("rules")) {
            auto rulesJson = configJson["rules"];
            for (const auto& ruleJson : rulesJson) {
                if (ruleJson.contains("pattern") && 
                    ruleJson.contains("response")) {
                    
                    std::string pattern = ruleJson["pattern"].get<std::string>();
                    std::string response = ruleJson["response"].get<std::string>();
                    int priority = 0;
                    
                    if (ruleJson.contains("priority")) {
                        priority = ruleJson["priority"].get<int>();
                    }
                    
                    AddRule(pattern, response, priority);
                }
            }
        }
        
        LoggingService::GetInstance().Log(
            LogLevel::INFO, 
            "RuleBasedAgent " + m_id + " initialized with " + 
            std::to_string(m_rules.size()) + " rules");
        
        return true;
    } 
    catch (const std::exception& e) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Failed to initialize RuleBasedAgent " + m_id + ": " + e.what());
        return false;
    }
}

std::string RuleBasedAgent::ProcessMessage(const std::string& message) {
    // Find the best matching rule
    const Rule* rule = FindMatchingRule(message);
    
    if (rule) {
        // Generate a response using the rule
        return GenerateRuleResponse(*rule, message);
    }
    
    // No matching rule, return default response
    return m_defaultResponse;
}

void RuleBasedAgent::so_define_agent() {
    // Subscribe to agent messages
    so_subscribe(m_mbox).event([this](const messages::AgentMessage& msg) {
        this->HandleMessage(msg);
    });
}

void RuleBasedAgent::so_evt_start() {
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "RuleBasedAgent " + m_id + " started");
}

void RuleBasedAgent::so_evt_finish() {
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "RuleBasedAgent " + m_id + " finished");
}

void RuleBasedAgent::AddRule(
    const std::string& pattern, 
    const std::string& responseTemplate, 
    int priority) {
    
    try {
        // Create a new rule
        Rule rule;
        rule.pattern = std::regex(pattern, std::regex::icase);
        rule.responseTemplate = responseTemplate;
        rule.priority = priority;
        
        // Add the rule to the list
        m_rules.push_back(rule);
        
        // Sort rules by priority (descending)
        std::sort(m_rules.begin(), m_rules.end(), 
                 [](const Rule& a, const Rule& b) {
                     return a.priority > b.priority;
                 });
        
        LoggingService::GetInstance().Log(
            LogLevel::DEBUG, 
            "Added rule with pattern '" + pattern + "' and priority " + 
            std::to_string(priority));
    }
    catch (const std::regex_error& e) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Invalid regex pattern '" + pattern + "': " + e.what());
    }
}

const Rule* RuleBasedAgent::FindMatchingRule(const std::string& message) const {
    // Try to match each rule in order of priority
    for (const auto& rule : m_rules) {
        if (std::regex_search(message, rule.pattern)) {
            return &rule;
        }
    }
    
    // No matching rule
    return nullptr;
}

std::string RuleBasedAgent::GenerateRuleResponse(
    const Rule& rule, 
    const std::string& message) const {
    
    // Simple template substitution
    std::string response = rule.responseTemplate;
    
    // Extract captures from the regex
    std::smatch matches;
    if (std::regex_search(message, matches, rule.pattern)) {
        // Replace $0, $1, etc. with the corresponding captures
        for (size_t i = 0; i < matches.size(); ++i) {
            std::string placeholder = "$" + std::to_string(i);
            std::string value = matches[i].str();
            
            // Replace all occurrences
            size_t pos = 0;
            while ((pos = response.find(placeholder, pos)) != std::string::npos) {
                response.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
    }
    
    return response;
}
void RuleBasedAgent::HandleMessage(const messages::AgentMessage& msg){

}

} // namespace ai_framework