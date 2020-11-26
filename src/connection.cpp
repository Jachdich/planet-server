#include "network.h"
#include "server.h"

Connection::Connection(asio::io_context& ctx, asio::ip::tcp::socket socket) : sock(std::move(socket)) {
    readUntil();
}
    
void Connection::handler(std::error_code ec, std::size_t bytes_transferred) {
    std::cout << bytes_transferred << "\n";
    if (!ec) {
        std::string request(buf.begin(), buf.begin() + bytes_transferred);
        buf.clear();

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
    }
    readUntil();
}

void Connection::readUntil() {
    asio::async_read_until(sock, asio::dynamic_buffer(buf), "\n", [this] (std::error_code ec, std::size_t bytes_transferred) {
        this->handler(ec, bytes_transferred);
    });
}