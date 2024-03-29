#include "connection.h"
#include "server.h"
#include <chrono>

Connection::Connection(asio::ssl::context& ctx, asio::ip::tcp::socket socket, ServerInterface *iface)
     : sock(std::move(socket), ctx) {
    this->uuid = 0;
    this->iface = iface;
    asio::error_code ec;
    sock.handshake(asio::ssl::stream_base::server, ec);
    if (ec) {
        logger.error("Connection Handshake error: " + ec.message());
        //close connection?
    } else {
        readUntil();
    }
}
    
void Connection::handler(asio::error_code ec, std::size_t bytes_transferred) {
    logger.debug("Read " + std::to_string(bytes_transferred) + " bytes");
    if (!ec) {
        std::string request{
                buffers_begin(buf.data()),
                buffers_begin(buf.data()) + bytes_transferred
                  - 1 /*for the \n*/};
        
        buf.consume(bytes_transferred);
        readUntil();

        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value root;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            request.c_str(),
            request.c_str() + request.size(),
            &root,
            &errors
        );
        delete reader;

        if (!parsingSuccessful) {
            logger.warn("Client sent malformed JSON request: " + request + ". Full error: " + errors);
            asio::error_code ign_error;
            asio::write(sock, asio::buffer("{\"status\": -1}\n"), ign_error);

        } else {
            this->handleRequest(root);
        }
    } else if ((ec == asio::error::eof) ||
            (ec == asio::error::connection_reset) ||
            (ec == asio::ssl::error::stream_truncated)) {
        disconnect();
    } else {
        logger.error("Connection error: " + ec.message());
        //readUntil();
        disconnect();
    }
}

void Connection::readUntil() {
    asio::async_read_until(sock, buf, '\n', [this] (asio::error_code ec, std::size_t bytes_transferred) {
        this->handler(ec, bytes_transferred);
    });
}

void Connection::sendMessage(Json::Value root) {
    asio::error_code err;
    Json::StreamWriterBuilder writeBuilder;
    writeBuilder["indentation"] = "";
    const std::string output = Json::writeString(writeBuilder, root);
    std::unique_lock<std::mutex> lock(mutex);
    asio::write(sock, asio::buffer(output + "\n"), err);
    lock.unlock();
    if (!err) {
        //success; potentially do something idk
    } else if ((err == asio::error::eof) ||
        (err == asio::error::connection_reset) ||
        (err == asio::ssl::error::stream_truncated)) {
        disconnect();
    } else {
        logger.error("Could not write request: " + err.message());
    }
}
