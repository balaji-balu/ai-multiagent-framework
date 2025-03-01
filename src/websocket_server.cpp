// websocket_server.cpp
#include "websocket_server.h"
#include "logging_service.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace ai_framework {

WebSocketServer::WebSocketServer(
    int port, 
    const std::string& cert_path, 
    const std::string& key_path)
    : m_port(port), 
      m_certPath(cert_path), 
      m_keyPath(key_path), 
      m_running(false) {
}

WebSocketServer::~WebSocketServer() {
    Stop();
}

bool WebSocketServer::Start() {
    if (m_running) {
        LoggingService::GetInstance().Log(
            LogLevel::WARNING, 
            "WebSocketServer already running");
        return true;
    }
    
    m_running = true;
    m_serverThread = std::thread(&WebSocketServer::ServerLoop, this);
    
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "WebSocketServer started on port " + std::to_string(m_port));
    
    return true;
}

void WebSocketServer::Stop() {
    if (!m_running) {
        return;
    }
    
    m_running = false;
    
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    
    LoggingService::GetInstance().Log(
        LogLevel::INFO, 
        "WebSocketServer stopped");
}

void WebSocketServer::SetMessageHandler(WebSocketMessageHandler handler) {
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_messageHandler = std::move(handler);
}

bool WebSocketServer::SendMessage(const std::string& client_id, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    auto it = m_clients.find(client_id);
    if (it == m_clients.end()) {
        LoggingService::GetInstance().Log(
            LogLevel::ERROR, 
            "Cannot send message: Client not found: " + client_id);
        return false;
    }
    
    // Cast void* back to WebSocket<SSL>*
    auto* ws = static_cast<uWS::WebSocket<false>*>(it->second);
    
    // Send the message
    ws->send(message, uWS::OpCode::TEXT);
    
    return true;
}

void WebSocketServer::Broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    
    for (const auto& pair : m_clients) {
        auto* ws = static_cast<uWS::WebSocket<false>*>(pair.second);
        ws->send(message, uWS::OpCode::TEXT);
    }
    
    LoggingService::GetInstance().Log(
        LogLevel::DEBUG, 
        "Broadcast message to " + std::to_string(m_clients.size()) + " clients");
}

void WebSocketServer::ServerLoop() {
    if (!m_certPath.empty() && !m_keyPath.empty()) {
        // Use SSL
        m_sslApp = std::make_unique<uWS::TemplatedApp<true>>(uWS::SSLApp({
            .key_file_name = m_keyPath.c_str(),
            .cert_file_name = m_certPath.c_str()
        }));
        
        // Configure WebSocket route
        m_sslApp->ws("/*", {
            // Connection opened
            .open = [this](auto* ws) {
                // Generate a client ID
                std::string client_id = this->GenerateClientId();
                
                // Store the WebSocket pointer
                {
                    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
                    this->m_clients[client_id] = ws;
                }
                
                // Store the client ID in user data
                ws->getUserData()->id = client_id;
                
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "Client connected: " + client_id);
            },
            
            // Message received
            .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
                if (opCode != uWS::OpCode::TEXT) {
                    return;
                }
                
                // Get the client ID from user data
                std::string client_id = ws->getUserData()->id;
                
                // Process the message
                std::lock_guard<std::mutex> lock(this->m_handlerMutex);
                if (this->m_messageHandler) {
                    this->m_messageHandler(
                        client_id, 
                        std::string(message), 
                        [ws](const std::string& response) {
                            ws->send(response, uWS::OpCode::TEXT);
                        });
                }
            },
            
            // Connection closed
            .close = [this](auto* ws, int code, std::string_view message) {
                // Get the client ID from user data
                std::string client_id = ws->getUserData()->id;
                
                // Remove the WebSocket pointer
                {
                    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
                    this->m_clients.erase(client_id);
                }
                
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "Client disconnected: " + client_id);
            }
        });
        
        // Listen on specified port
        m_sslApp->listen(m_port, [this](auto* listen_socket) {
            if (listen_socket) {
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "SSL WebSocket server listening on port " + 
                    std::to_string(this->m_port));
            }
            else {
                LoggingService::GetInstance().Log(
                    LogLevel::ERROR, 
                    "Failed to listen on port " + 
                    std::to_string(this->m_port));
            }
        });
        
        // Run the event loop
        m_sslApp->run();
    }
    else {
        // Use non-SSL
        m_app = std::make_unique<uWS::TemplatedApp<false>>(uWS::App());
        
        // Configure WebSocket route
        m_app->ws("/*", {
            // Connection opened
            .open = [this](auto* ws) {
                // Generate a client ID
                std::string client_id = this->GenerateClientId();
                
                // Store the WebSocket pointer
                {
                    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
                    this->m_clients[client_id] = ws;
                }
                
                // Store the client ID in user data
                ws->getUserData()->id = client_id;
                
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "Client connected: " + client_id);
            },
            
            // Message received
            .message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
                if (opCode != uWS::OpCode::TEXT) {
                    return;
                }
                
                // Get the client ID from user data
                std::string client_id = ws->getUserData()->id;
                
                // Process the message
                std::lock_guard<std::mutex> lock(this->m_handlerMutex);
                if (this->m_messageHandler) {
                    this->m_messageHandler(
                        client_id, 
                        std::string(message), 
                        [ws](const std::string& response) {
                            ws->send(response, uWS::OpCode::TEXT);
                        });
                }
            },
            
            // Connection closed
            .close = [this](auto* ws, int code, std::string_view message) {
                // Get the client ID from user data
                std::string client_id = ws->getUserData()->id;
                
                // Remove the WebSocket pointer
                {
                    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
                    this->m_clients.erase(client_id);
                }
                
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "Client disconnected: " + client_id);
            }
        });
        
        // Listen on specified port
        m_app->listen(m_port, [this](auto* listen_socket) {
            if (listen_socket) {
                LoggingService::GetInstance().Log(
                    LogLevel::INFO, 
                    "WebSocket server listening on port " + 
                    std::to_string(this->m_port));
            }
            else {
                LoggingService::GetInstance().Log(
                    LogLevel::ERROR, 
                    "Failed to listen on port " + 
                    std::to_string(this->m_port));
            }
        });
        
        // Run the event loop
        m_app->run();
    }
}

std::string WebSocketServer::GenerateClientId() const {
    // Use a random number generator to create a unique ID
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    // Generate a UUID-like string
    std::stringstream ss;
    ss << std::hex;
    
    for (int i = 0; i < 8; ++i) {
        ss << dis(gen);
    }
    ss << "-";
    
    for (int i = 0; i < 4; ++i) {
        ss << dis(gen);
    }
    ss << "-4";
    
    for (int i = 0; i < 3; ++i) {
        ss << dis(gen);
    }
    ss << "-";
    
    ss << (dis(gen) & 0x3 | 0x8);
    
    for (int i = 0; i < 3; ++i) {
        ss << dis(gen);
    }
    ss << "-";
    
    for (int i = 0; i < 12; ++i) {
        ss << dis(gen);
    }
    
    return ss.str();
}

} // namespace ai_framework
