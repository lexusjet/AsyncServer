#pragma once

#include <boost/asio.hpp>

class SensorMessage;

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
    using ErrorCollback = std::function<void (const int, const ErrorCode)>;
    using StateChangedCollback = std::function<void (const int, const Connection::State)>;
    using HendleMessage = std::function<void (SensorMessage&)>;

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

    void write(const SensorMessage& anser);
    
    void onWrite(const ErrorCode& error, const size_t transferd);

    void onTimeout(const ErrorCode& error);

    void close();
    
};