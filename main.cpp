#include "server.hpp"
int main() {
    boost::asio::io_context io_context;
    server srv(io_context, 15002);
    srv.async_accept();
    boost::asio::signal_set signals(io_context, SIGINT);
    io_context.run();
    return 0;
}