#include <gtest/gtest.h>

#include <TestClient/TestClient.h>
#include <AsyncServer/AsyncServer.h>

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
int curentPort = 1000;

TEST(AsyncServer, incorect_addres)
{
    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };

    tcp::endpoint endpoint(asio::ip::make_address("127.0.0.0"), 1000);
    AsyncServer server(
        endpoint,
        serverErrorCollback
    );
    EXPECT_EQ(server.getState(), AsyncServer::FailedToConstruct);
    curentPort++;
}

TEST(AsyncServer, ServerCreated)
{
    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server (
        endpoint,
        serverErrorCollback
    );
    EXPECT_EQ(server.getState(), AsyncServer::Created);
    curentPort++;
}

TEST(AsyncServer, ServerListening)
{
    AsyncServer::ErrorCollback serverErrorCollback = 
    [](const ErrorCode& error)
    {
        EXPECT_TRUE(error);
    };
    tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
    AsyncServer server (
        endpoint,
        serverErrorCollback
    );
    server.run();
    EXPECT_EQ(server.getState(), AsyncServer::Listening);
    curentPort++;
}

// TEST(AsyncServer, Server_stop)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
// }

// TEST(AsyncServer, Server_stop_start)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
// }

TEST(AsyncServer_Connection, recive_data)
{
    std::thread serverThread(
        [&]()
        {
            AsyncServer::ErrorCollback serverErrorCollback = 
            [](const ErrorCode& error)
            {
                EXPECT_FALSE(error);
            };
            tcp::endpoint endpoint(asio::ip::make_address(corectIP), curentPort);
            AsyncServer server(endpoint, serverErrorCollback);
            EXPECT_EQ(server.getState(), AsyncServer::Created);
            server.run();
        }
    );
    sleep(1);
    std::string message = "hellow server\n";
    std::string anser;
    std::thread clientThread(
        [&](){
            asio::io_context clientContext;
            tcp::endpoint endP(asio::ip::make_address(corectIP), curentPort);
            std::shared_ptr<TestClient> client = std::make_shared<TestClient>(
                clientContext,
                endP,
                [&](const ErrorCode& error){
                    EXPECT_FALSE(error);
                    client->reciveMessage();
                },
                [&](const ErrorCode& error, const std::string& str){
                    EXPECT_FALSE(error);
                    anser = str;
                }
            );
            client->sendMessage(message);
            clientContext.run();
        }
    );
    
    clientThread.join();
    serverThread.join();

    EXPECT_EQ(message, anser);
    curentPort++;
}

// TEST(AsyncServer_Connection, disconect_while_riding)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     EXPECT_EQ(server->getState(), AsyncServer::Created);
//     server->run();
//     std::thread serverThread([&](){context.run();});
//     std::string message = "hellow server\n";
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
//                     client->disconetc();
//                 },
//                 [&](const ErrorCode& error, const std::string& str){
//                     EXPECT_FALSE(true);
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

// TEST(AsyncServer_Connection, disconect_while_writing)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 2001);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     server->run();
//     EXPECT_EQ(server->getState(), AsyncServer::Listening);
// }

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
