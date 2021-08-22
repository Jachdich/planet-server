#ifndef __SERVERINTERFACE_H
#define __SERVERINTERFACE_H
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <vector>
#include <jsoncpp/json/json.h>
#include <mutex>
#include "user.h"
#include "connection.h"

class ServerInterface {
private:

    asio::io_context ctx;
    asio::ssl::context sslCtx;
    
    std::thread threadCtx;
    asio::ip::tcp::acceptor acceptor;

public:
    typedef std::shared_ptr<Connection> Conn;
    std::vector<Conn> connections;
    std::unordered_map<std::string, uint64_t> nameToUUID;
    std::unordered_map<uint64_t, UserMetadata> accounts; //lol this is inefficient
                                                         //but I don't care!

    ServerInterface(uint16_t port);
    ~ServerInterface();

	void startServer();
    void waitForClientConnection();
    void saveUsers();
};
#endif