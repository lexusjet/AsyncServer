#include <TestClient/TestClient.h>

TestClient::TestClient(
        asio::io_context& context,
        tcp::endpoint endpoint,
        ErrorCallback errorCallback,
        MessageSendCallback sendCallback,
        MessageReciveCallback reciveCallback
    ):
        m_socket(context),
        onError(errorCallback),
        onMessageSend(sendCallback),
        onMessageRecived(reciveCallback)
{
    try
    {
        m_socket.connect(endpoint);
    }
    catch(...)
    {
        onError(ErrorCode());
    }
    
}   


void TestClient::sendMessage(const std::string& message)
{
    std::ostream os(&m_otputBuffer);
    os << message;

    asio::async_write(
        m_socket,
        m_otputBuffer,
        boost::bind(
            &TestClient::onWrite,
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::results
        )
    );
}

void TestClient::reciveMessage()
{
    asio::async_read_until(
        m_socket,
        m_inputBuffer,
        "\n",
        boost::bind(
            &TestClient::onRecive,
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::results
        )
    );
}

void TestClient::onWrite(const ErrorCode& error, size_t transferd)
{
    if (error)
    {
        onError(error);
        return;
    }
    onMessageSend(error);
    m_otputBuffer.consume(transferd);
    reciveMessage();
}

void TestClient::onRecive(const ErrorCode& error, const size_t transferd)
{
    if(error)
    {
        onError(error);
        return;
    }
    
    auto data = m_inputBuffer.data();
    std::string str(
        asio::buffers_begin(data),
        asio::buffers_begin(data) + transferd
    );

    onMessageRecived(error, str);
}

void TestClient::disconetc()
{
    m_socket.close();
}
