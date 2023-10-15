#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

namespace asio = boost::asio;

class TestClient : public std::enable_shared_from_this<TestClient>
{
private:
    using tcp = boost::asio::ip::tcp;
    using ErrorCode = boost::system::error_code;

public:
    using ErrorCallback = std::function<void (ErrorCode)>;
    using MessageSendCallback = std::function<void (const ErrorCode)>;
    using MessageReciveCallback = std::function<void (const ErrorCode&, const std::string&)>;

private:
    tcp::socket m_socket;
    ErrorCallback onError;
    MessageSendCallback onMessageSend;
    MessageReciveCallback onMessageRecived;
    asio::streambuf m_otputBuffer;
    asio::streambuf m_inputBuffer;

public:
    TestClient(
            asio::io_context& context,
            tcp::endpoint endpoint,
            ErrorCallback errorCallback,
            MessageSendCallback sendCallback,
            MessageReciveCallback reciveCallback
        );

    void sendMessage(const std::string& message);

    void reciveMessage();

    void disconetc();
private:
    
    void onRecive(const ErrorCode& error, const size_t transferd);

    void onWrite(const ErrorCode& error, size_t transferd);
};