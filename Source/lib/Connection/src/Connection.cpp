#include <Connection/Connection.h>
#include <chrono>

Connection::Connection(
    tcp::socket&& soket,
    ErrorCollback errorCollback,
    StateChangedCollback stateCollback,
    HendleMessage handler
):
    m_socket(std::move(soket)),
    m_deadLineTimer(m_socket.get_executor(), std::chrono::seconds(2)),
    onErrorCollback(errorCollback),
    onStateChangedCallback(stateCollback),
    hendleMessage(handler)
{
    m_state = Connected;
    onStateChangedCallback(m_state);
}

Connection::Connection(Connection&& connection):
    m_socket(std::move(connection.m_socket)),
    m_deadLineTimer(m_socket.get_executor(), connection.m_deadLineTimer.expires_at()),
    m_state(connection.m_state),
    m_isLoging(connection.m_isLoging)
{

}

void Connection::run()
{
    m_deadLineTimer.async_wait(
        boost::bind(
            &Connection::onTimeout,
            this,
            asio::placeholders::error
        )
    );
    asio::dispatch(
        m_socket.get_executor(),
        boost::bind(
            &Connection::onRun,
            this
        )
    );
}

void Connection::onRun()
{
    read();
}

void Connection::read()
{
    asio::async_read_until(
        m_socket,
        m_inputBuffer,
        "\n",
        boost::bind(
            &Connection::onRead,
            this,
            asio::placeholders::error,
            asio::placeholders::results
        )
    );
    // asio::async_read_until(
    //     m_socket,
    //     m_inputBuffer,
    //     "\n",
    //     boost::asio::bind_cancellation_slot()
    // );
    m_state = AsyncReadyng;
    onStateChangedCallback(m_state);
};

void Connection::onRead(const ErrorCode& error, const size_t transferd)
{
    if(error)
    {
        onErrorCollback(error);
        return;
    }
    auto data = m_inputBuffer.data();
    std::string str(
        asio::buffers_begin(data),
        asio::buffers_begin(data) + transferd
    );
    std::cout << str << std::endl;
    write(hendleMessage(str));
};

void Connection::write(const std::string& anser)
{
    std::ostream os(&m_outputBuffer);
    os << anser;

    asio::async_write(
        m_socket,
        m_outputBuffer,
        boost::bind(
            &Connection::onWrite,
            this,
            asio::placeholders::error,
            asio::placeholders::results
        )
    );
    m_state = AsyncWriting;
    onStateChangedCallback(m_state);
};

void Connection::onWrite(const ErrorCode& error, const size_t transferd)
{
    if (error)
    {
        onErrorCollback(error);
        return;
    }
    m_outputBuffer.consume(transferd);
    close();
}

void Connection::onTimeout(const ErrorCode& error)
{
    m_state = Timeout;
    onStateChangedCallback(m_state);
}

void Connection::close()
{
    m_socket.close();
    m_state = Closed;
    onStateChangedCallback(m_state);
}
