#include "network.h"

ServerInterface::ServerInterface(uint16_t port) : sslCtx(asio::ssl::context::tls),
                                                  acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
}

void ServerInterface::startServer() {
	sslCtx.use_private_key_file("server.key", asio::ssl::context::pem);
    sslCtx.use_certificate_chain_file("server.crt");
    sslCtx.use_tmp_dh_file("dh2048.pem");
    //sslCtx.set_options(asio::ssl::context::default_workarounds);
    try {
        waitForClientConnection();
        threadCtx = std::thread([this]() {ctx.run(); });
        logger.info("Server Started");
        runServerLogic();
    } catch (std::exception& e) {
        logger.error("Starting server: " + std::string(e.what()));
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
                std::stringstream ss;
                ss << socket.remote_endpoint();
                logger.info("New connection: " + ss.str());
                
                Conn newConn = std::make_shared<Connection>(sslCtx, std::move(socket), IDCounter++);
                connections.push_back(newConn);
            } else {
                logger.error("New connection error: " + ec.message());
            }
            waitForClientConnection();
        }
    );
}

void ServerInterface::messageClient(Conn con, const Json::Value& msg) {
    
}

void ServerInterface::messageAll(const Json::Value& msg, Conn ignoreClient) {
    
}
