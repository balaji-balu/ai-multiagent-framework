// websocket_server.h
#ifndef AI_FRAMEWORK_WEBSOCKET_SERVER_H
#define AI_FRAMEWORK_WEBSOCKET_SERVER_H

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <App.h>

namespace ai_framework {

/**
 * @brief Callback type for websocket message handlers
 */
using WebSocketMessageHandler = std::function<
    void(const std::string&, const std::string&, std::function<void(const std::string&)>)>;

/**
 * @brief WebSocket server for external communication
 * 
 * This class provides a WebSocket interface for external clients
 * to communicate with the agent system.
 */
class WebSocketServer {
public:
    /**
     * @brief Constructor for WebSocketServer
     * 
     * @param port Port to listen on
     * @param cert_path Path to SSL certificate file (optional)
     * @param key_path Path to SSL key file (optional)
     */
    WebSocketServer(
        int port, 
        const std::string& cert_path = "", 
        const std::string& key_path = "");
    
    /**
     * @brief Destructor for WebSocketServer
     */
    ~WebSocketServer();
    
    /**
     * @brief Start the server
     * 
     * @return bool True if the server started successfully, false otherwise
     */
    bool Start();
    
    /**
     * @brief Stop the server
     */
    void Stop();
    
    /**
     * @brief Set the message handler
     * 
     * @param handler Function to handle incoming websocket messages
     */
    void SetMessageHandler(WebSocketMessageHandler handler);
    
    /**
     * @brief Send a message to a specific client
     * 
     * @param client_id ID of the client to send to
     * @param message Message to send
     * @return bool True if the message was sent, false otherwise
     */
    bool SendMessage(const std::string& client_id, const std::string& message);
    
    /**
     * @brief Broadcast a message to all connected clients
     * 
     * @param message Message to broadcast
     */
    void Broadcast(const std::string& message);

private:
    /** Port to listen on */
    int m_port;
    
    /** Path to SSL certificate file */
    std::string m_certPath;
    
    /** Path to SSL key file */
    std::string m_keyPath;
    
    /** Flag indicating if the server is running */
    bool m_running;
    
    /** Server thread */
    std::thread m_serverThread;
    
    /** Message handler */
    WebSocketMessageHandler m_messageHandler;
    
    /** Mutex for thread-safe access to the handler */
    std::mutex m_handlerMutex;
    
    /** Map of client IDs to websocket connections */
    std::map<std::string, void*> m_clients;
    
    /** Mutex for thread-safe access to clients */
    std::mutex m_clientsMutex;
    
    /** uWebSockets app instance */
    std::unique_ptr<uWS::TemplatedApp<false>> m_app;
    
    /** uWebSockets SSL app instance */
    std::unique_ptr<uWS::TemplatedApp<true>> m_sslApp;
    
    /**
     * @brief Server loop function
     */
    void ServerLoop();
    
    /**
     * @brief Generate a unique client ID
     * 
     * @return std::string Unique client ID
     */
    std::string GenerateClientId() const;
};

} // namespace ai_framework

#endif // AI_FRAMEWORK_WEBSOCKET_SERVER_H
