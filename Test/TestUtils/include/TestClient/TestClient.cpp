#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

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
        // onError(ErrorCode());
        return -1;
    }
    return 0;
    }

    int sendMessage(const std::string& message)
    {
    
    // std::ostream os(&m_otputBuffer);
    // os << message;
    size_t sended = 0;
    while (sended < 5)
    {
        try
        {
            // sended += m_socket.send(m_otputBuffer);
            sended += m_socket.send(asio::buffer(message));
        }
        catch(...)
        {
            return -1;
        }

    }
    return 0;

    // asio::async_write(
    //     m_socket,
    //     m_otputBuffer,
    //     boost::bind(
    //         &TestClient::onWrite,
    //         shared_from_this(),
    //         asio::placeholders::error,
    //         asio::placeholders::results
    //     )
    // );
    }

    int reciveMessage(std::string answer)
    {
        size_t recived = 0;
        char recv_str[5];
        // socket.receive(boost::asio::buffer(recv_str));
        try
        {
            while (recived < 1)
            {
                // recived += m_socket.receive(m_inputBuffer);
                recived += m_socket.receive(boost::asio::buffer(recv_str));
                
            }
            
        }
        catch(const std::exception& e)
        {
            return -1;
        }
        return 0;

        // auto data = m_inputBuffer.data();
        // std::string reply(
        //     asio::buffers_begin(data),
        //     asio::buffers_begin(data) + recived
        // );
        // answer = reply;

        // asio::async_read_until(
        //     m_socket,
        //     m_inputBuffer,
        //     "\n",
        //     boost::bind(
        //         &TestClient::onRecive,
        //         shared_from_this(),
        //         asio::placeholders::error,
        //         asio::placeholders::results
        //     )
        // );
    }

    void disconetc()
    {
        m_socket.close();
    }
private:
    
    void onRecive(const ErrorCode& error, const size_t transferd)
    {
    // if(error)
    // {
    //     onError(error);
    //     return;
    // }
    
    // auto data = m_inputBuffer.data();
    // std::string str(
    //     asio::buffers_begin(data),
    //     asio::buffers_begin(data) + transferd
    // );

    // onMessageRecived(error, str);
    }

    void onWrite(const ErrorCode& error, size_t transferd)
    {
    // if (error)
    // {
    //     onError(error);
    //     return;
    // }
    // onMessageSend(error);
    // m_otputBuffer.consume(transferd);
    // reciveMessage();
    }
};