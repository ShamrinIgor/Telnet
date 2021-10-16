#include <iostream>
#include <optional>
#include <boost/asio.hpp>

namespace ba = boost::asio;

class session : public std::enable_shared_from_this<session> {
public:
    session(ba::ip::tcp::socket&& socket, ba::io_context& io_context) : socket(std::move(socket)), io_context(io_context) {}
    void start();
    std::string /*void*/ read();
    void write();

private:
    ba::ip::tcp::socket socket;
    ba::streambuf streambuf;
    ba::io_context& io_context;
};

class server {
public:
    server(ba::io_context& io_context, std::uint16_t port) : io_context(io_context), acceptor(io_context, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port)) {}

    void async_accept();

private:
    ba::io_context& io_context;
    ba::ip::tcp::acceptor acceptor;
    std::optional<ba::ip::tcp::socket> socket;
};
