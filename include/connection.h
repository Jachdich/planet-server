#ifndef __CONNECTION_H
#define __CONNECTION_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <vector>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <mutex>
#include "planetsurface.h"

class Connection : public std::enable_shared_from_this<Connection> {
private:
    asio::ssl::stream<asio::ip::tcp::socket> sock;
    asio::streambuf buf;
    std::mutex mutex;

public:
    std::vector<PlanetSurface*> surfacesLoaded;

    uint64_t uuid;
    Connection(asio::ssl::context& ctx, asio::ip::tcp::socket socket);

    void handleRequest(Json::Value& request);
    void sendMessage(Json::Value root);
    
private:
    void handler(asio::error_code ec, std::size_t bytes_transferred);
    void readUntil();
    void disconnect();
};

#endif