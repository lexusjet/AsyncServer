#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>
#include <SensorMessage/SensorMessage.h>

namespace asio = boost::asio;

// class TestClient : public std::enable_shared_from_this<TestClient>
class TestClient
{
private:
    using tcp = boost::asio::ip::tcp;
    using ErrorCode = boost::system::error_code;

public:
    using ErrorCallback = std::function<void (ErrorCode)>;
    using MessageSendCallback = std::function<void (const ErrorCode)>;
    using MessageReciveCallback = std::function<void (const ErrorCode&, const std::string&)>;

private:
    asio::io_context m_ioContext;
    tcp::socket m_socket;
    // ErrorCallback onError;
    // MessageSendCallback onMessageSend;
    // MessageReciveCallback onMessageRecived;
    asio::streambuf m_otputBuffer;
    asio::streambuf m_inputBuffer;

public:
    TestClient():m_socket(m_ioContext){}

    int connect(const tcp::endpoint& endpoint)
    {
    try
    {
        m_socket.connect(endpoint);
    }
    catch(...)
    {
        return -1;
    }
    return 0;
    }

    int sendMessage(const SensorMessage& message)
    {

        size_t sended = 0;
        std::vector<char> buffer(message.messegeSize());
        for (size_t i = 0; i < buffer.size(); ++i)
        {
            buffer[i] = (message.data())[i];
        }

        while (sended < MESSAGE_SIZE)
        {
            try
            {
                sended += m_socket.send(asio::buffer(buffer));
            }
            catch(...)
            {
                return -1;
            }

        }
        return 0;

    }

    int reciveMessage(SensorMessage& answer)
    {
        size_t recived = 0;
        char recv_str[MESSAGE_SIZE];
        try
        {
            while (recived < MESSAGE_SIZE)
            {
                recived += m_socket.receive(boost::asio::buffer(recv_str));
            }
            
        }
        catch(const std::exception& e)
        {
            return -1;
        }

        char* p = (char*)(&answer);
        for (size_t i = 0; i < MESSAGE_SIZE; ++i)
        {
            *(p + i) = recv_str[i];
        }

        return 0;

    }

    void disconect()
    {
        m_socket.close();
    }
private:
    
};