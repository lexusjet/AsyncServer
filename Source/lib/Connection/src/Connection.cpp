#include <Connection/Connection.h>
#include <chrono>
#include <iostream>
#include <boost/asio/strand.hpp>
#include <SensorMessage/SensorMessage.h>

Connection::Connection(
    tcp::socket&& soket,
    ErrorCollback errorCollback,
    StateChangedCollback stateCollback,
    HendleMessage handler
):
    m_socket(std::move(soket)),
    m_inputBuffer(MESSAGE_SIZE),
    m_outputBuffer(MESSAGE_SIZE),
    m_deadLineTimer(m_socket.get_executor(), std::chrono::seconds(2)),
    m_state(Connected),
    onErrorCollback(errorCollback),
    onStateChangedCallback(stateCollback),
    hendleMessage(handler)
{
    
    onStateChangedCallback(static_cast<int>(m_socket.native_handle()), m_state);
}

Connection::Connection(Connection&& connection):
    m_socket(std::move(connection.m_socket)),
    m_inputBuffer(MESSAGE_SIZE),
    m_outputBuffer(MESSAGE_SIZE),
    m_deadLineTimer(m_socket.get_executor(), connection.m_deadLineTimer.expires_at()),
    m_state(connection.m_state),
    m_isLoging(connection.m_isLoging),
    onErrorCollback(connection.onErrorCollback),
    onStateChangedCallback(connection.onStateChangedCallback),
    hendleMessage(connection.hendleMessage)
{

}

void Connection::run()
{ 
    // m_deadLineTimer.async_wait(
    //     std::bind(
    //         &Connection::onTimeout,
    //         this,
    //         std::placeholders::_1
    //     )
    // );
    asio::post(
        m_socket.get_executor(),
        std::bind(&Connection::onRun, this)
    );

}

void Connection::onRun()
{
    read();
}

void Connection::read()
{
    asio::async_read(
        m_socket,
        m_inputBuffer,
        std::bind(
            &Connection::onRead,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
    m_state = AsyncReadyng;
    onStateChangedCallback(static_cast<int>(m_socket.native_handle()), m_state);
};

void Connection::onRead(const ErrorCode& error, const size_t transferd)
{
    if(error)
    {
        onErrorCollback(static_cast<int>(m_socket.native_handle()), error);
        return;
    }
    std::istream is(&m_inputBuffer);
    SensorMessage message;
    is >> message;
    
    hendleMessage(message);
    write(message);
};

void Connection::write(const SensorMessage& anser)
{
    std::ostream os(&m_outputBuffer);
    os << anser;

    asio::async_write(
        m_socket,
        m_outputBuffer,
        std::bind(
            &Connection::onWrite,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
    m_state = AsyncWriting;
    onStateChangedCallback(static_cast<int>(m_socket.native_handle()) ,m_state);
};

void Connection::onWrite(const ErrorCode& error, const size_t transferd)
{
    if (error)
    {
        onErrorCollback(static_cast<int>(m_socket.native_handle()), error);
        return;
    }
    m_outputBuffer.consume(transferd);
    close();
}

void Connection::onTimeout(const ErrorCode& error)
{
    m_state = Timeout;
    onStateChangedCallback(static_cast<int>(m_socket.native_handle()), m_state);
}

void Connection::close()
{
    m_socket.close();
    m_state = Closed;
    onStateChangedCallback(static_cast<int>(m_socket.native_handle()), m_state);
}
