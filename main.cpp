#include <cstdio>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/process/system.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;

#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

namespace ba = boost::asio;
namespace bp = boost::process;

awaitable<void> telnet(tcp::socket socket)
{
    try
    {
        ba::io_context io_context;
        bp::environment e = boost::this_process::environment();
        bp::child c(e.at("SHELL").to_string(), io_context, bp::start_dir = "/Users/igor/cringe/telnet/cmake-build-debug");
        for (;;)
        {
            std::string s;
            bp::ipstream ipipe;
            std::string line;
            std::string result;
            ba::streambuf streambuf;

            co_await  ba::async_read_until(socket, streambuf, '\n', use_awaitable);

            std::istream is(&streambuf);
            std::getline(is, s);
            streambuf.consume(s.size());

            bp::system(io_context, s, bp::std_out > ipipe);

            while (std::getline(ipipe, line) && !line.empty()) {
                result.append(line);
                result.append("\n");
            }

            co_await async_write(socket, ba::buffer(result, result.size()), use_awaitable);
            result.clear();
        }
    }
    catch (std::exception& e)
    {
        std::printf("telnet Exception: %s\n", e.what());
    }
}

awaitable<void> listener()
{
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 15002});
    for (;;)
    {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, telnet(std::move(socket)), detached);
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context(1);

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_context.stop(); });

        co_spawn(io_context, listener(), detached);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::printf("Exception: %s\n", e.what());
    }
}