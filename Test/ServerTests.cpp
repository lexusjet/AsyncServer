#include <gtest/gtest.h>

// #include <TestClient/TestClient.h>
// #include "../Test/TestUtils/Test"
#include <AsyncServer/AsyncServer.h>
#include <TestClient/TestClient.cpp>
#include <TestListner/TestListner.cpp>

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>
#include <chrono>
#include <memory>
#include <time.h>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
using ErrorCode = boost::system::error_code;
const std::string corectIP = "127.0.0.1";
int curentPort = 2000;


AsyncServer::ErrorCollback DefaultServerErrorCollback = 
[](const ErrorCode& error)
{
    EXPECT_TRUE(error);
};


/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
TEST(AsyncServer, incorect_addres)
{
    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    AsyncServer::StateChangedCollback func =
    [](const AsyncServer::State state){};

    tcp::endpoint endpoint(asio::ip::make_address("127.0.0.0"), 1000);
    AsyncServer server(
        endpoint,
        func,
        serverErrorCollback
    );
    EXPECT_EQ(server.getState(), AsyncServer::FailedToConstruct);
    curentPort++;
}

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
TEST(AsyncServer, ServerCreated)
{
    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_FALSE(error);
    };
    AsyncServer::StateChangedCollback func =
    [](const AsyncServer::State state){};

    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server (
        endpoint,
        func,
        serverErrorCollback
    );
    EXPECT_EQ(server.getState(), AsyncServer::Created);
    curentPort++;
}

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
TEST(AsyncServer, ServerListening)
{
    std::condition_variable cv;
    std::atomic<bool> flag = false;

    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    AsyncServer::StateChangedCollback func =
    [&](const AsyncServer::State state)
    {
        flag.store(true);
        flag.notify_all();
    };

    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server (
        endpoint,
        func,
        serverErrorCollback
    );
    std::thread thr (&AsyncServer::run, &server);
    
    flag.wait(false);
    EXPECT_EQ(server.getState(), AsyncServer::Listening);
    server.stop("");
    thr.join();
    curentPort++;
}

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
// TEST(AsyncServer, Server_stop)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
// }

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
// TEST(AsyncServer, Server_stop_start)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
// }

/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */
TEST(AsyncServer_Connection, recive_data)
{
    SensorMessage outMessage;
    outMessage.setData(1);
    outMessage.setDataType(2);
    outMessage.setLocation(3);
    outMessage.setExtension(4);
    outMessage.setNumberInLocation(5);
    outMessage.setReserv(6);
    outMessage.setVersion(7);

    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    AsyncServer::StateChangedCollback func = [&](const AsyncServer::State state){};

    TestListner listner([&](SensorMessage& message){EXPECT_EQ(message, outMessage);});

    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server(
        endpoint,
        func,
        serverErrorCollback
    );
    std::thread thr(&AsyncServer::run, &server);

    std::thread clientThread(
        [&](){
            TestClient client;
            SensorMessage inMessage;
            EXPECT_NE(client.connect(endpoint), -1);
            EXPECT_NE(client.sendMessage(outMessage), -1);
            EXPECT_NE(client.reciveMessage(inMessage), -1);
            EXPECT_EQ(outMessage, inMessage);    
        }
    );

    clientThread.join();
    server.stop("");
    thr.join();
}

/* =================================================================== */
TEST(AsyncServer_Connection, disconect_while_riding)
{
    SensorMessage outMessage;
    outMessage.setData(1);
    outMessage.setDataType(2);
    outMessage.setLocation(3);
    outMessage.setExtension(4);
    outMessage.setNumberInLocation(5);
    outMessage.setReserv(6);
    outMessage.setVersion(7);

    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    AsyncServer::StateChangedCollback func = [&](const AsyncServer::State state){};

    TestListner listner([&](SensorMessage& message){EXPECT_EQ(message, outMessage);});

    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server(
        endpoint,
        func,
        serverErrorCollback
    );
    std::thread thr(&AsyncServer::run, &server);

    std::thread clientThread(
        [&](){
            TestClient client;
            SensorMessage inMessage;
            EXPECT_NE(client.connect(endpoint), -1);
            client.disconect();
        }
    );

    clientThread.join();
    server.stop("");
    thr.join();
}

TEST(AsyncServer_Connection, disconect_while_writing)
{
    std::atomic<bool> flag = false;
    SensorMessage outMessage;
    outMessage.setData(1);
    outMessage.setDataType(2);
    outMessage.setLocation(3);
    outMessage.setExtension(4);
    outMessage.setNumberInLocation(5);
    outMessage.setReserv(6);
    outMessage.setVersion(7);

    AsyncServer::ErrorCollback serverErrorCollback = 
    [&](const ErrorCode& error)
    {
        EXPECT_TRUE(flag.load());
        EXPECT_TRUE(error);
    };
    AsyncServer::StateChangedCollback func = [&](const AsyncServer::State state){};

    TestListner listner(
        [&](SensorMessage& message)
        {
            EXPECT_EQ(message, outMessage);
            flag.store(true);
            flag.notify_all();
        }
    );

    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server(
        endpoint,
        func,
        serverErrorCollback
    );
    std::thread thr(&AsyncServer::run, &server);

    std::thread clientThread(
        [&](){
            TestClient client;
            SensorMessage inMessage;
            EXPECT_NE(client.connect(endpoint), -1);
            EXPECT_NE(client.sendMessage(outMessage), -1);
            flag.wait(true);
            client.disconect();
        }
    );

    clientThread.join();
    server.stop("");
    thr.join();
}

// TEST(AsyncServer_Connection, timeout_check)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     EXPECT_EQ(server->getState(), AsyncServer::Created);
//     server->run();
//     std::thread serverThread([&](){context.run();});
//     std::string message = "hellow server";
//     std::string anser;
//     std::thread clientThread(
//         [&](){
//             asio::io_context clientContext;
//             tcp::endpoint endP(asio::ip::make_address("127.0.0.1"), 2001);
//             std::shared_ptr<TestClient> client = std::make_shared<TestClient>(
//                 clientContext,
//                 endP,
//                 [&](const ErrorCode& error){
//                     EXPECT_FALSE(error);
//                     client->reciveMessage();
//                 },
//                 [&](const ErrorCode& error, const std::string& str){
//                     EXPECT_TRUE(error);
//                 }
//             );
//             client->sendMessage(message);
//             clientContext.run();
//         }
//     );
    
//     clientThread.join();
//     context.stop();
//     serverThread.join();

//     EXPECT_EQ(anser, "");
// }

// TEST(AsyncServer_Connection, Server_stoped_while_connections_is_workin)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
//     EXPECT_EQ(server->getState(), AsyncServer::Listening);
// }
