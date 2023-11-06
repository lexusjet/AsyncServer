#include <AsyncServer/AsyncServer.h>

#include <iostream>
#include <memory>
#include <chrono>

#include <boost/bind/bind.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
using ErrorCode = boost::system::error_code;
//=======================================================================

AsyncServer::AsyncServer(
        tcp::endpoint endPoint,
        StateChangedCollback stateChangedCollback,
        ErrorCollback errorCollback
):
    m_acceptor(m_ioContext),
    m_onStateChangedCollback(stateChangedCollback),
    m_onErrorCollback(errorCollback)
{
    m_state = State::Constructing;
    ErrorCode errorCode;

    // Open the acceptor
    m_acceptor.open(endPoint.protocol(), errorCode);
    if(errorCode)
    {
        m_state = FailedToConstruct;
        m_onErrorCollback(errorCode);
        return;
    }

    // Allow address reuse
    m_acceptor.set_option(
        asio::socket_base::reuse_address(true),
        errorCode
    );

    if(errorCode)
    {
        m_state = FailedToConstruct;
        m_onErrorCollback(errorCode);
        return;
    }

    // Bind to the server address
    m_acceptor.bind(endPoint, errorCode);
    if(errorCode)
    {
        m_state = FailedToConstruct;
        m_onErrorCollback(errorCode);
        return;
    }

    // Start listening for connections
    m_acceptor.listen(
        asio::socket_base::max_listen_connections,
        errorCode
    );
    if(errorCode)
    {
        m_state = FailedToConstruct;
        m_onErrorCollback(errorCode);
        return;
    }

    std::unique_lock lock(m_mutex);
    m_state = State::Created;
}

/* =================================================================== */
AsyncServer::~AsyncServer()
{
    m_state = State::Destroyed;
    for (auto listner: m_listenerVec)
    {
        listner->unSubscribe();
    }
}

/* =================================================================== */
AsyncServer::State AsyncServer::getState()
{
    std::unique_lock lock(m_mutex);
    return m_state;
}

/* =================================================================== */
void AsyncServer::run()
{
    // asio::strand strand = asio::make_strand(m_ioContext);
    m_ioContext.post(
        std::bind(
            [&](){
                this->doAccept();
                m_state.store(Listening);
                m_onStateChangedCollback(Listening);
            }
        )
    );
    // m_ioContext.post(boost::bind(&AsyncServer::doAccept, this));
    // m_ioContext.post(std::bind(m_onStateChangedCollback, Listening));
    m_ioContext.run();
}

void AsyncServer::stop(const std::string& stopMode)
{
    if (stopMode == "soft")
    {
        
    }
    else if (stopMode == "hard")
    {

    }
    else
    {
    }
    m_ioContext.stop();

}

/* =================================================================== */
void AsyncServer::addListner(ServerListener* pListner)
{
    m_listenerVec.emplace_back(pListner);
    pListner->subscribe(this);
}

/* =================================================================== */
void AsyncServer::removeListner(ServerListener* pListner)
{
    auto itr = std::find(m_listenerVec.begin(), m_listenerVec.end(), pListner);
    if (itr != m_listenerVec.end())
    {
        m_listenerVec.erase(itr);
        pListner->unSubscribe();
    }
}

/* =================================================================== */
void AsyncServer::doAccept()
{

    m_acceptor.async_accept(
        asio::make_strand(m_ioContext),
        beast::bind_front_handler(
            &AsyncServer::onAccept,
            this
        )
    );
}

/* =================================================================== */
void AsyncServer::onAccept(
    const boost::system::error_code& errorCode,
    tcp::socket sock
)
{
    if(errorCode)
    {
        m_onErrorCollback(errorCode);
        return;
    }
    else
    {
        const int socketDsk = static_cast<int>(sock.native_handle());
        auto connectionIt = m_connectionsUMap.emplace
        (
            socketDsk,
            Connection(
                std::move(sock),
                [this, socketDsk](const ErrorCode error){
                    this->connectionErrorHandler(socketDsk, error);
                },
                [this, socketDsk](const Connection::State state){
                    this->connectionStateHandler(socketDsk, state);
                },
                [](const std::string& str){
                    return str;
                }
            )
        );
        connectionIt.first->second.run();
    }
    doAccept();
}

/* =================================================================== */
void AsyncServer::connectionErrorHandler(const int socket, const ErrorCode error)
{
    m_connectionsUMap.erase(socket);
}

/* =================================================================== */
void AsyncServer::connectionStateHandler(const int socket, const Connection::State state)
{
    switch (state)
    {
        case Connection::Connected:
            
            break;
        case Connection::Disconected:
            
            break;
        case Connection::AsyncReadyng:
            
            break;
        case Connection::AsyncWriting:
            
            break;
        case Connection::Timeout:
            m_connectionsUMap.erase(socket);
            break;
        case Connection::Closed:
            m_connectionsUMap.erase(socket);
            break;

        default:
            break;
    }
}


