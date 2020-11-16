#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
class Connection {
private:
    asio::ip::tcp::socket sock;
    std::vector<uint8_t> buf;

public:
    Connection(asio::io_context& ctx, asio::ip::tcp::socket socket);

    void handleRequest(Json::Value& request);
    
private:
    void handler(std::error_code ec, std::size_t bytes_transferred);
    void readUntil();
};

class ServerInterface {
private:

    asio::io_context ctx;
    std::thread threadCtx;
    asio::ip::tcp::acceptor acceptor;

    uint32_t IDCounter = 0;
    
    typedef std::shared_ptr<Connection> Conn;
    std::vector<Conn> connections;

public:
    ServerInterface(uint16_t port);
    ~ServerInterface();

    void waitForClientConnection();
    void messageClient(Conn client, const Json::Value& msg);
    void messageAll(const Json::Value& msg, Conn ignoreClient);
};

#endif