#include "Message.h"
#include <stdexcept>

Message::Message() : m_type(MessageType::CHAT), m_timestamp(std::chrono::system_clock::now()) {}

Message::Message(MessageType type, const std::string& username, const std::string& content)
    : m_type(type), m_username(username), m_content(content), m_timestamp(std::chrono::system_clock::now()) {}

std::string Message::serialize() const {
    nlohmann::json j;
    
    j["type"] = static_cast<int>(m_type);
    j["username"] = m_username;
    j["content"] = m_content;
    j["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        m_timestamp.time_since_epoch()).count();
    
    return j.dump();
}

Message Message::deserialize(const std::string& json) {
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        
        Message msg;
        msg.m_type = static_cast<MessageType>(j["type"].get<int>());
        msg.m_username = j["username"].get<std::string>();
        msg.m_content = j["content"].get<std::string>();
        
        auto timestamp_ms = j["timestamp"].get<long long>();
        msg.m_timestamp = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp_ms));
        
        return msg;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to deserialize message: " + std::string(e.what()));
    }
}