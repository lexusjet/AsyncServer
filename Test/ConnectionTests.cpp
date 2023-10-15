#include <gtest/gtest.h>

#include <TestClient/TestClient.h>
#include <AsyncServer/AsyncServer.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <time.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
using ErrorCode = boost::system::error_code;
using ErrorCollback = std::function<void (const ErrorCode)>;
using StateChangedCollback = std::function<void (const Connection::State)>;
using HendleMessage = std::function<void (const std::string&, std::string&)>;

void initAcceptor(tcp::acceptor& acceptor, tcp::endpoint endpoint)
{
    ErrorCode errorCode;

    acceptor.open(endpoint.protocol(), errorCode);
    ASSERT_FALSE(errorCode) << "open failed";
    acceptor.set_option(
        asio::socket_base::reuse_address(true),
        errorCode
    );
    ASSERT_FALSE(errorCode) << "set option";
    acceptor.bind(endpoint, errorCode);
    ASSERT_FALSE(errorCode) << "bind failed";
    acceptor.listen(
        asio::socket_base::max_listen_connections,
        errorCode
    );
    ASSERT_FALSE(errorCode) << "listen failed";
}


TEST(Connection, defualt)
{
    asio::io_context context;
    tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4977);
    boost::asio::ip::tcp::acceptor acceptor(context);
    initAcceptor(acceptor, endpoint);
    std::unique_ptr<Connection> pConnection;
    std::string message = "hi alex\n";
    
    std::condition_variable cv;
    std::atomic<bool> connerctionFlag = false;

    ErrorCollback errorCollback = [](const ErrorCode error)
    {
        ASSERT_FALSE(error) << "error collback";
    };

    StateChangedCollback stateChangedCollback = [](const Connection::State state)
    {

    };

    HendleMessage hendleMessage = [&](const std::string& answer, std::string& reply)
    {
        reply = answer;
        ASSERT_EQ(answer, message);
    };


    asio::io_context::work work(context);

    std::thread connectionThread(
        [&](){
            tcp::socket newSock(context);
            acceptor.async_accept(
                newSock,
                [&](const ErrorCode errorCodenewSock){
                    pConnection = std::make_unique<Connection>(Connection(
                            std::move(newSock),
                            errorCollback,
                            stateChangedCollback,
                            hendleMessage
                        )
                    );
                    pConnection->run();
                }
            );

            context.run();
        }
    );

    asio::io_context clientContext;
    std::atomic<bool> flag  = false;
    std::shared_ptr<TestClient> client = std::make_shared<TestClient>(
        clientContext,
        endpoint,
        [](const ErrorCode error)
        {
            ASSERT_FALSE(error) << "client error";
        },
        [](const ErrorCode error)
        {
            ASSERT_FALSE(error);
        },
        [&](const ErrorCode error, const std::string& answer)
        {
            flag.store(true);
            cv.notify_all();
            ASSERT_FALSE(error);
            ASSERT_EQ(answer, message);
        }
    );
    client->sendMessage(message);

    std::thread clientThrea([&](){clientContext.run();});

    std::mutex mtx;
    std::unique_lock lock(mtx);
    cv.wait(lock, [&](){return flag.load();});
    context.stop();
    clientThrea.join();
    connectionThread.join();
    
    
}

