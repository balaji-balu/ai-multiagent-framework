// messages.h
#ifndef AI_FRAMEWORK_MESSAGES_H
#define AI_FRAMEWORK_MESSAGES_H

#include <string>
#include <so_5/all.hpp>

namespace ai_framework {
namespace messages {

/**
 * @brief Message for agent creation
 */
struct AgentCreate final : public so_5::message_t {
    /** Type of agent to create */
    std::string type;
    
    /** ID for the new agent */
    std::string id;
    
    /** Configuration for the new agent */
    std::string config;
    
    /**
     * @brief Constructor for AgentCreate message
     * 
     * @param t Type of agent to create
     * @param i ID for the new agent
     * @param c Configuration for the new agent
     */
    AgentCreate(std::string t, std::string i, std::string c)
        : type(std::move(t)), id(std::move(i)), config(std::move(c)) {}
};

/**
 * @brief Message for agent destruction
 */
struct AgentDestroy final : public so_5::message_t {
    /** ID of the agent to destroy */
    std::string id;
    
    /**
     * @brief Constructor for AgentDestroy message
     * 
     * @param i ID of the agent to destroy
     */
    explicit AgentDestroy(std::string i) : id(std::move(i)) {}
};

/**
 * @brief Message for agent communication
 */
struct AgentMessage final : public so_5::message_t {
    /** ID of the source agent */
    std::string sourceId;
    
    /** ID of the target agent */
    std::string targetId;
    
    /** Content of the message */
    std::string content;
    
    /** Mbox for sending back the response */
    so_5::mbox_t replyTo;
    
    /**
     * @brief Constructor for AgentMessage
     * 
     * @param src ID of the source agent
     * @param tgt ID of the target agent
     * @param cnt Content of the message
     * @param reply Mbox for sending back the response
     */
    AgentMessage(
        std::string src,
        std::string tgt,
        std::string cnt,
        so_5::mbox_t reply)
        : sourceId(std::move(src)),
          targetId(std::move(tgt)),
          content(std::move(cnt)),
          replyTo(std::move(reply)) {}
};

/**
 * @brief Message for agent response
 */
struct AgentResponse final : public so_5::message_t {
    /** ID of the responding agent */
    std::string agentId;
    
    /** Response content */
    std::string content;
    
    /**
     * @brief Constructor for AgentResponse
     * 
     * @param id ID of the responding agent
     * @param cnt Response content
     */
    AgentResponse(std::string id, std::string cnt)
        : agentId(std::move(id)), content(std::move(cnt)) {}
};

} // namespace messages
} // namespace ai_framework

#endif // AI_FRAMEWORK_MESSAGES_H