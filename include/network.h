#ifndef __NETWORK_H
#define __NETWORK_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <vector>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <mutex>

#include "sectormap.h"

extern SectorMap map;

class Connection : public std::enable_shared_from_this<Connection> {
private:
    asio::ssl::stream<asio::ip::tcp::socket> sock;
    asio::streambuf buf;
    std::mutex mutex;

public:
    std::vector<PlanetSurface*> surfacesLoaded;

    uint32_t id;
    Connection(asio::ssl::context& ctx, asio::ip::tcp::socket socket, uint32_t id);

    void handleRequest(Json::Value& request);
    void sendMessage(Json::Value root);
    
private:
    void handler(asio::error_code ec, std::size_t bytes_transferred);
    void readUntil();
    void disconnect();
};

class ServerInterface {
private:

    asio::io_context ctx;
    asio::ssl::context sslCtx;
    
    std::thread threadCtx;
    asio::ip::tcp::acceptor acceptor;

    uint32_t IDCounter = 0;

public:
    typedef std::shared_ptr<Connection> Conn;
    std::vector<Conn> connections;

    ServerInterface(uint16_t port);
    ~ServerInterface();

	void startServer();
    void waitForClientConnection();
    void messageClient(Conn client, const Json::Value& msg);
    void messageAll(const Json::Value& msg, Conn ignoreClient);
};

void runServerLogic();

#include "server.h"
#include "common/surfacelocator.h"

void sendResourcesChangeRequest(Resources resources, SurfaceLocator loc);
void registerTaskTypeInfo();
void sendTileChangeRequest(uint32_t pos, TileType type, SurfaceLocator loc);
void sendTileErrorSetRequest(SurfaceLocator loc, uint32_t index, std::string err);
void sendSetTimerRequest(double time, uint32_t target, SurfaceLocator loc);

#endif
