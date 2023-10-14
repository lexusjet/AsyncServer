#pragma once
#include <Connection/Connection.h>
// класс сервера должен осуществлять покдлючение новых соединений 
// выдавать ошибки в своей работе
// 
// минимальная версия должна уметь подключить соединение 
// и отключить как только оно закончиться
#include <string>
#include <boost/asio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <condition_variable>


// =============================================================================
class ServerListner;

class AsyncServer
{
public:
        enum State{
            Constructing,
            FailedToConstruct,
            Created,
            Listening,
            Stopped,
            ListenStopped,
            Destroyed,
        };
    using ErrorCode = boost::system::error_code;
    using ErrorCollback = std::function<void (const ErrorCode& )>;

private:
    boost::asio::io_context m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    
    // curent state of server
    State m_state;
    
    ErrorCollback onErrorCollback;

    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;    

    std::unordered_map<int, Connection> m_connectionsUMap;
public:
    AsyncServer() = delete;
    AsyncServer(const AsyncServer&) = delete;
    void operator=(const AsyncServer& ) = delete;


    AsyncServer(
        boost::asio::ip::tcp::endpoint,
        ErrorCollback
    );
    ~AsyncServer();

    State getState();

    void run(); 

    void stop(const std::string& stopMode);

private:
    void doAccept();

    void onAccept(
        const ErrorCode&, 
        boost::asio::ip::tcp::socket socket
    );

    void connectionStateHandler(const int socket, const Connection::State state);
    
    void connectionErrorHandler(const int socket, const ErrorCode error);
};

