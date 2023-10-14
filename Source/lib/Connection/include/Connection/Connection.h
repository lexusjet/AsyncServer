#pragma once

#include <iostream>
#include <boost/bind/bind.hpp>
#include <memory>
#include <boost/asio/strand.hpp>
#include <boost/asio.hpp>


//========================================================================

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
using ErrorCode = boost::system::error_code;

class Connection
{
public:
    enum State{
        Connected,
        Disconected,
        AsyncReadyng,
        AsyncWriting,
        Timeout,
        Closed,
        StopesAccepting,
        AcceptingStoped

    };
    // 
    using ErrorCollback = std::function<void (const ErrorCode)>;
    using StateChangedCollback = std::function<void (const Connection::State)>;
    
    using HendleMessage = std::function<std::string (const std::string&)>;

    // using LogActions
    using LogerCollback = std::function<void (const std::string&)>;

private:
    tcp::socket m_socket;
    asio::streambuf m_inputBuffer;
    asio::streambuf m_outputBuffer;
    asio::steady_timer m_deadLineTimer;

    State m_state;
    bool m_isLoging;
    

    ErrorCollback onErrorCollback;
    StateChangedCollback onStateChangedCallback;
    LogerCollback onLogerCollback;
    HendleMessage hendleMessage;

public:
    Connection(
        tcp::socket&& soket,
        ErrorCollback errorCollback,
        StateChangedCollback stateCollback,
        HendleMessage handler
    );

    Connection(const Connection&) = delete;
    void operator=(const Connection&) = delete;
    Connection(Connection&&);
    void operator=(Connection&&) = delete;

    void run();

private:
    void onRun();

    void read();

    void onRead(const ErrorCode& error, const size_t size);

    void write(const std::string& anser);
    
    void onWrite(const ErrorCode& error, const size_t transferd);

    void onTimeout(const ErrorCode& error);

    void close();
    
};