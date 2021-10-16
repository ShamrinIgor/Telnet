#include "server.hpp"
#include <iostream>
#include <string>
#include <boost/process.hpp>
#include <boost/process/system.hpp>
#include <boost/asio/spawn.hpp>
//#include "spawn.hpp"

namespace bp = boost::process;
namespace ba = boost::asio;

bool isEqualTo(std::string it, std::string that) {
    std::transform(it.begin(), it.end(), it.begin(), tolower);
    return (it.compare(that) == 0);
}

void session::start() {
    bp::environment e = boost::this_process::environment();
    bp::child c(e.at("SHELL").to_string(), io_context, bp::start_dir = "/Users/igor/cringe/telnet/cmake-build-debug");
    ba::spawn(io_context, [&c, this, self = shared_from_this()](ba::yield_context yc) {
        boost::system::error_code ec;

        for(;;) {
            std::string s;
            bp::ipstream ipipe;
            std::string line;
            std::string result;

            ba::async_read_until(socket, streambuf, '\n', yc[ec]);
            if (!ec) {
                std::istream is(&streambuf);
                std::getline(is, s);
                if (isEqualTo(s, "kys")) { c.terminate(); }
                streambuf.consume(s.size());

                bp::system(io_context, s, bp::std_out > ipipe);

                std::getline(ipipe, line);

                while (std::getline(ipipe, line) && !line.empty()) {
                    result.append(line);
                    result.append("\n");
                }
            }

            std::cout << "s is: " << s;
            ba::async_write(socket, ba::buffer(result, result.size()), yc[ec]);
            if (ec) { std::cout << "not read\n"; } else {
                result.clear();
            }
        }
        });

}

void server::async_accept() {
    socket.emplace(io_context);
    //ba::spawn(io_context, [this, &socket](yield_context yield){});
    acceptor.async_accept(*socket, [&](boost::system::error_code error) {
        std::make_shared<session>(std::move(*socket), io_context)->start();
        async_accept();
    });
}