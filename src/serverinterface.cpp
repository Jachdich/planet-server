#include "network.h"

ServerInterface::ServerInterface(uint16_t port) : acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    try {
        waitForClientConnection();
        //threadCtx = std::thread([this]() {ctx.run(); });
        std::cout << "[SERVER] Started\n";
        ctx.run();
    } catch (std::exception& e) {
        std::cerr << "[SERVER] Exception: " << e.what() << "\n";
    }
}

ServerInterface::~ServerInterface() {
    ctx.stop();
    if (threadCtx.joinable()) threadCtx.join();
}

void ServerInterface::waitForClientConnection() {
    acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[SERVER] New connection: " << socket.remote_endpoint() << "\n";
                Conn newConn = std::make_shared<Connection>(ctx, std::move(socket));
                connections.push_back(newConn);
            } else {
                std::cerr << "[SERVER] New connection error: " << ec.message() << "\n";
            }
            waitForClientConnection();
        }
    );
}

void ServerInterface::messageClient(Conn client, const Json::Value& msg) {
    
}

void ServerInterface::messageAll(const Json::Value& msg, Conn ignoreClient) {
    
}