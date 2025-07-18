#pragma once

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

enum class MessageType {
    CHAT,
    SYSTEM,
    USER_JOIN,
    USER_LEAVE
};

class Message {
public:
    Message();
    Message(MessageType type, const std::string& username, const std::string& content);
    
    MessageType getType() const { return m_type; }
    const std::string& getUsername() const { return m_username; }
    const std::string& getContent() const { return m_content; }
    std::chrono::system_clock::time_point getTimestamp() const { return m_timestamp; }
    
    void setType(MessageType type) { m_type = type; }
    void setUsername(const std::string& username) { m_username = username; }
    void setContent(const std::string& content) { m_content = content; }
    void setTimestamp(std::chrono::system_clock::time_point timestamp) { m_timestamp = timestamp; }
    
    std::string serialize() const;
    static Message deserialize(const std::string& json);
    
private:
    MessageType m_type;
    std::string m_username;
    std::string m_content;
    std::chrono::system_clock::time_point m_timestamp;
};