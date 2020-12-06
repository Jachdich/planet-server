#include "network.h"

ServerInterface::ServerInterface(uint16_t port) : sslCtx(asio::ssl::context::tls),
                                                  acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
}

void ServerInterface::startServer() {
	sslCtx.use_private_key_file("server.key", asio::ssl::context::pem);
    sslCtx.use_certificate_chain_file("server.crt");
    sslCtx.use_tmp_dh_file("dh2048.pem");
    try {
        waitForClientConnection();
        threadCtx = std::thread([this]() {ctx.run(); });
        std::cout << "[SERVER] Started\n";
        runServerLogic();
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
                
                Conn newConn = std::make_shared<Connection>(sslCtx, std::move(socket), IDCounter++);
                connections.push_back(newConn);
            } else {
                std::cerr << "[SERVER] New connection error: " << ec.message() << "\n";
            }
            waitForClientConnection();
        }
    );
}

void ServerInterface::messageClient(Conn con, const Json::Value& msg) {
    
}

void ServerInterface::messageAll(const Json::Value& msg, Conn ignoreClient) {
    
}
