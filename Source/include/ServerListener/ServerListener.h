#pragma once
/* =================================================================== */
#include <string>

class AsyncServer;
class SensorMessage;

class ServerListener
{

protected:
    AsyncServer* m_server = nullptr;

public:
    ServerListener(){};
    virtual ~ServerListener(){};


    virtual void subscribe(AsyncServer* pServer)
    {
        m_server = pServer;
    }
    
    virtual void unSubscribe()
    {
        m_server = nullptr;
    }

    virtual void notify(SensorMessage& message) = 0;

};