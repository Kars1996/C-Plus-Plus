#pragma once

#include "Message.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include <thread>
#include <mutex>
#include <memory>

class ChatServer {
public:
    ChatServer(int port);
    ~ChatServer();
    
    void start();
    void stop();
    
private:
    typedef websocketpp::server<websocketpp::config::asio> server_type;
    typedef websocketpp::connection_hdl connection_hdl;
    typedef server_type::message_ptr message_ptr;
    
    void onOpen(connection_hdl hdl);
    void onClose(connection_hdl hdl);
    void onMessage(connection_hdl hdl, message_ptr msg);
    void broadcastMessage(const Message& message, connection_hdl sender = connection_hdl());
    
    server_type m_server;
    std::thread m_serverThread;
    std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
    std::mutex m_connectionMutex;
    int m_port;
    bool m_running;
};