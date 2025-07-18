#pragma once

#include "Message.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class ChatClient {
public:
    ChatClient(const std::string& host, int port);
    ~ChatClient();
    
    bool connect();
    void run();
    void disconnect();
    
private:
    typedef websocketpp::client<websocketpp::config::asio_client> client_type;
    typedef websocketpp::connection_hdl connection_hdl;
    typedef client_type::message_ptr message_ptr;
    
    void onOpen(connection_hdl hdl);
    void onClose(connection_hdl hdl);
    void onMessage(connection_hdl hdl, message_ptr msg);
    void onFail(connection_hdl hdl);
    
    void sendMessage(const std::string& content);
    void inputLoop();
    
    client_type m_client;
    connection_hdl m_connection;
    std::string m_host;
    int m_port;
    std::string m_username;
    std::thread m_inputThread;
    std::atomic<bool> m_connected;
    std::atomic<bool> m_running;
    std::mutex m_outputMutex;
};