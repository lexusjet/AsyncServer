#include <string>


class AsyncServer;

class ServerListener
{

protected:
    AsyncServer* m_server = nullptr;

public:


    virtual void subscribe(AsyncServer* pServer)
    {
        m_server = pServer;
    }
    
    virtual void unSubscribe()
    {
        m_server = nullptr;
    }

    virtual void notify(std::string& message) = 0;

    virtual ~ServerListener();
};