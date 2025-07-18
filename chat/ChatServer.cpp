#include "ChatServer.h"
#include <iostream>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

ChatServer::ChatServer(int port) : m_port(port), m_running(false) {
    m_server.set_access_channels(websocketpp::log::alevel::all);
    m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_server.set_error_channels(websocketpp::log::elevel::all);
    
    m_server.init_asio();
    m_server.set_reuse_addr(true);
    
    m_server.set_open_handler(std::bind(&ChatServer::onOpen, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&ChatServer::onClose, this, std::placeholders::_1));
    m_server.set_message_handler(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

ChatServer::~ChatServer() {
    stop();
}

void ChatServer::start() {
    if (m_running) return;
    
    m_server.listen(m_port);
    m_server.start_accept();
    m_running = true;
    
    m_serverThread = std::thread([this]() {
        try {
            m_server.run();
        } catch (const std::exception& e) {
            std::cerr << "Server error: " << e.what() << std::endl;
        }
    });
    
    std::cout << "Chat server started on port " << m_port << std::endl;
}

void ChatServer::stop() {
    if (!m_running) return;
    
    m_running = false;
    m_server.stop();
    
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    
    std::cout << "Chat server stopped" << std::endl;
}

void ChatServer::onOpen(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(m_connectionMutex);
    m_connections.insert(hdl);
    
    std::cout << "Client connected. Total connections: " << m_connections.size() << std::endl;
    
    Message welcomeMsg;
    welcomeMsg.setType(MessageType::SYSTEM);
    welcomeMsg.setContent("Welcome to the uwu chat server!");
    welcomeMsg.setTimestamp(std::chrono::system_clock::now());
    
    try {
        m_server.send(hdl, welcomeMsg.serialize(), websocketpp::frame::opcode::text);
    } catch (const std::exception& e) {
        std::cerr << "Error sending welcome message: " << e.what() << std::endl;
    }
}

void ChatServer::onClose(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(m_connectionMutex);
    m_connections.erase(hdl);
    
    std::cout << "Client disconnected. Total connections: " << m_connections.size() << std::endl;
}

void ChatServer::onMessage(connection_hdl hdl, message_ptr msg) {
    try {
        Message chatMsg = Message::deserialize(msg->get_payload());
        
        std::cout << "[" << chatMsg.getUsername() << "]: " << chatMsg.getContent() << std::endl;
        broadcastMessage(chatMsg, hdl);
    } catch (const std::exception& e) {
        std::cerr << "Error processing message: " << e.what() << std::endl;
    }
}

void ChatServer::broadcastMessage(const Message& message, connection_hdl sender) {
    std::lock_guard<std::mutex> lock(m_connectionMutex);
    
    std::string serialized = message.serialize();
    
    for (auto it = m_connections.begin(); it != m_connections.end();) {
        try {
            if (sender.lock() && it->lock() == sender.lock()) {
                ++it;
                continue;
            }
            
            m_server.send(*it, serialized, websocketpp::frame::opcode::text);
            ++it;
        } catch (const std::exception& e) {
            std::cerr << "Error broadcasting to client: " << e.what() << std::endl;
            it = m_connections.erase(it);
        }
    }
}