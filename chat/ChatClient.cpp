#include "ChatClient.h"
#include <iostream>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

ChatClient::ChatClient(const std::string& host, int port) 
    : m_host(host), m_port(port), m_connected(false), m_running(false) {
    
    m_client.set_access_channels(websocketpp::log::alevel::all);
    m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_client.set_error_channels(websocketpp::log::elevel::all);
    
    m_client.init_asio();
    
    m_client.set_open_handler(std::bind(&ChatClient::onOpen, this, std::placeholders::_1));
    m_client.set_close_handler(std::bind(&ChatClient::onClose, this, std::placeholders::_1));
    m_client.set_message_handler(std::bind(&ChatClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_client.set_fail_handler(std::bind(&ChatClient::onFail, this, std::placeholders::_1));
}

ChatClient::~ChatClient() {
    disconnect();
}

bool ChatClient::connect() {
    try {
        std::string uri = "ws://" + m_host + ":" + std::to_string(m_port);
        
        websocketpp::lib::error_code ec;
        auto con = m_client.get_connection(uri, ec);
        
        if (ec) {
            std::cerr << "Connection error: " << ec.message() << std::endl;
            return false;
        }
        
        m_connection = con->get_handle();
        m_client.connect(con);
        
        std::thread clientThread([this]() {
            m_client.run();
        });
        clientThread.detach();
        
        for (int i = 0; i < 50 && !m_connected; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        if (!m_connected) {
            std::cerr << "Connection timeout" << std::endl;
            return false;
        }
        
        std::cout << "Enter your username: ";
        std::getline(std::cin, m_username);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return false;
    }
}

void ChatClient::run() {
    if (!m_connected) return;
    
    m_running = true;
    m_inputThread = std::thread(&ChatClient::inputLoop, this);
    if (m_inputThread.joinable()) {
        m_inputThread.join();
    }
}

void ChatClient::disconnect() {
    if (!m_connected) return;
    
    m_running = false;
    m_connected = false;
    
    try {
        m_client.close(m_connection, websocketpp::close::status::normal, "Client disconnecting");
    } catch (const std::exception& e) {
        std::cerr << "Error during disconnect: " << e.what() << std::endl;
    }
    
    if (m_inputThread.joinable()) {
        m_inputThread.join();
    }
}

void ChatClient::onOpen(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    m_connected = true;
    std::cout << "Connected to chat server!" << std::endl;
}

void ChatClient::onClose(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    m_connected = false;
    m_running = false;
    std::cout << "Disconnected from chat server." << std::endl;
}

void ChatClient::onMessage(connection_hdl hdl, message_ptr msg) {
    try {
        Message chatMsg = Message::deserialize(msg->get_payload());
        
        std::lock_guard<std::mutex> lock(m_outputMutex);
        auto time = std::chrono::system_clock::to_time_t(chatMsg.getTimestamp());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        
        if (chatMsg.getType() == MessageType::SYSTEM) {
            std::cout << "[SYSTEM] " << chatMsg.getContent() << std::endl;
        } else {
            std::cout << "[" << ss.str() << "] " << chatMsg.getUsername() << ": " << chatMsg.getContent() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing received message: " << e.what() << std::endl;
    }
}

void ChatClient::onFail(connection_hdl hdl) {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    std::cout << "Connection failed." << std::endl;
    m_connected = false;
    m_running = false;
}

void ChatClient::sendMessage(const std::string& content) {
    if (!m_connected) return;
    
    try {
        Message msg;
        msg.setType(MessageType::CHAT);
        msg.setUsername(m_username);
        msg.setContent(content);
        msg.setTimestamp(std::chrono::system_clock::now());
        
        m_client.send(m_connection, msg.serialize(), websocketpp::frame::opcode::text);
        
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
}

void ChatClient::inputLoop() {
    std::string input;
    
    while (m_running && m_connected) {
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            m_running = false;
            break;
        }
        if (!input.empty()) {
            sendMessage(input);
        }
    }
    
    disconnect();
}