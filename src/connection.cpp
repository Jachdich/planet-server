#include "network.h"
#include "server.h"
#include <chrono>

Connection::Connection(asio::ssl::context& ctx, asio::ip::tcp::socket socket, uint32_t id) : sock(std::move(socket), ctx) {
    this->id = id;
    sock.handshake(asio::ssl::stream_base::server);
    readUntil();
}
    
void Connection::handler(std::error_code ec, std::size_t bytes_transferred) {
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
    } else {
        std::cerr << "ERROR: " <<  ec.message() << "\n";
        readUntil();
    }
}

void Connection::readUntil() {
    asio::async_read_until(sock, buf, '\n', [this] (std::error_code ec, std::size_t bytes_transferred) {
        this->handler(ec, bytes_transferred);
    });
}