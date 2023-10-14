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

// TEST(ghg, incorect_addres)
// {
//     asio::io_context context;
//     tcp::endpoint endpoint(asio::ip::make_address("127.0.0.0"), 1000);
//     std::shared_ptr<AsyncServer> server = std::make_shared<AsyncServer>(context, endpoint);
//     EXPECT_EQ(server->getState(), AsyncServer::FailedToConstruct);
// }

