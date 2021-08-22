#include "serverinterface.h"
#include "logging.h"
#include "server.h"
#include "tick.h"
#include <fstream>
ServerInterface::ServerInterface(uint16_t port) : sslCtx(asio::ssl::context::tls),
                                                  acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    Json::Value root;
    std::ifstream ifs;
    ifs.open(saveName + "/users.json");

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        logger.error("Could not read /users.json!");
    }

    for (Json::Value val : root["users"]) {
        UserMetadata x;
        x.fromJson(val);
        accounts[val["uuid"].asUInt64()] = x;
        nameToUUID[val["name"].asString()] = val["uuid"].asUInt64();
    }
}

void ServerInterface::saveUsers() {
    /*std::ofstream afile;
	afile.open(saveName + "/users.json");
	Json::StreamWriterBuilder writeBuilder;
	writeBuilder["indentation"] = "";
	afile << Json::writeString(writeBuilder, this->asJson()) << "\n";
	afile.close();*/ //fuck it
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
                
                Conn newConn = std::make_shared<Connection>(sslCtx, std::move(socket));
                connections.push_back(newConn);
            } else {
                logger.error("New connection error: " + ec.message());
            }
            waitForClientConnection();
        }
    );
}