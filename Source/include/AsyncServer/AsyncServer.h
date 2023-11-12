#pragma once

// класс сервера должен осуществлять покдлючение новых соединений 
// выдавать ошибки в своей работе
// 
// минимальная версия должна уметь подключить соединение 
// и отключить как только оно закончиться

#include <Connection/Connection.h>
#include <boost/asio.hpp>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

// =============================================================================
class ServerListener;
class Connection;
class SensorMessage;


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
    using StateChangedCollback = std::function<void (const State)>;
    using ErrorCollback = std::function<void (const ErrorCode& )>;

private:
    boost::asio::io_context m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    
    // curent state of server
    // State m_state;
    std::atomic<State> m_state;

    StateChangedCollback m_onStateChangedCollback;
    ErrorCollback m_onErrorCollback;

    std::mutex m_mutex;
    std::condition_variable m_conditionVariable;    

    std::unordered_map<int, Connection> m_connectionsUMap;

    std::vector<ServerListener*> m_listenerVec;

public:
    AsyncServer() = delete;
    AsyncServer(const AsyncServer&) = delete;
    void operator=(const AsyncServer& ) = delete;

    AsyncServer(
        boost::asio::ip::tcp::endpoint,
        StateChangedCollback,
        ErrorCollback
    );
    ~AsyncServer();

    State getState();
    size_t nomberOfSessions();

    void run(); 

    void stop(const std::string& stopMode);

    void addListner(ServerListener* pListner);
    void removeListner(ServerListener* pListner);

private:

    void doAccept();

    void onAccept(
        const ErrorCode&, 
        boost::asio::ip::tcp::socket socket
    );

    void notifyListners(SensorMessage& ) const;
    
    void connectionStateHandler(const int socket, const Connection::State state);
    
    void connectionErrorHandler(const int socket, const ErrorCode error);
};

