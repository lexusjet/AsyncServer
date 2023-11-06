#pragma once

// #include <ServerListener/ServerListener.h>
#include <AsyncServer/AsyncServer.h>
#include <functional>


class TestListner : public ServerListener 
{
    std::function<void (std::string& )> m_onNotifyCallback;

public:
    TestListner(std::function<void (std::string& )> func)
        :m_onNotifyCallback(func)
    {};
    
    virtual void notify(std::string& message) override
    {
        m_onNotifyCallback(message);
    };

    virtual ~TestListner()
    {
        if (m_server != nullptr)
        {
            m_server->removeListner(this);
        }
    }

};