#pragma once

// #include <ServerListener/ServerListener.h>
#include <AsyncServer/AsyncServer.h>
#include <functional>
#include <ServerListener/ServerListener.h>

class TestListner : public ServerListener 
{
    std::function<void (SensorMessage& )> m_onNotifyCallback;

public:

    TestListner(std::function<void (SensorMessage& )> func)
        :m_onNotifyCallback(func)
    {};
    
    virtual void notify(SensorMessage& message) override
    {
        m_onNotifyCallback(message);
    };

    ~TestListner()
    {
        if (m_server != nullptr)
        {
            m_server->removeListner(this);
        }
    }

};