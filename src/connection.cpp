#include "network.h"
#include "server.h"
#include <chrono>

Connection::Connection(asio::ssl::context& ctx, asio::ip::tcp::socket socket, uint32_t id) : sock(std::move(socket), ctx) {
    this->id = id;
    asio::error_code ec;
    sock.handshake(asio::ssl::stream_base::server, ec);
    if (ec) {
        std::cout << "[CONNECTION] Handshake error: " << ec.message() << "\n";
        //close connection?
    } else {
        readUntil();
    }
}
    
void Connection::handler(asio::error_code ec, std::size_t bytes_transferred) {
    std::cout << bytes_transferred << "\n";
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
        std::cerr << "ERROR: " <<  ec.message() << "\n";
        readUntil();
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
    asio::write(sock, asio::buffer(output + "\n"), err);
    if (!err) {
        //success; potentially do something idk
    } else if ((err == asio::error::eof) ||
        (err == asio::error::connection_reset) ||
        (err == asio::ssl::error::stream_truncated)) {
        disconnect();
    } else {
        std::cout << "[CONNECTION] Could not write request. Error: " << err.message() << "\n";
    }
}
