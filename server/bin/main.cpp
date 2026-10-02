#include <iostream>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "core.hpp"

namespace po = boost::program_options;
using namespace std;

int main(int ac, char **av) {
    po::options_description desc("Usage: cosmocraft-server");
    desc.add_options()("help", "show this help")(
        "bind_addr", po::value<std::string>()->default_value("127.0.0.1"))(
        "port", po::value<unsigned short>()->default_value(8080));

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    spdlog::set_pattern("[%5t] [%H:%M:%S] %v");
    spdlog::cfg::load_env_levels();

    auto bind_addr = vm["bind_addr"].as<std::string>();
    auto port = vm["port"].as<unsigned short>();

    auto instance = cosmo::Instance::launch(port, bind_addr);

    // boost::asio::io_context io;
    // boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    // signals.async_wait([&](const std::error_code &ec, int sig) {
    //     if (!ec)
    //         std::cout << "signal " << sig << " reçu, arrêt...\n";
    //     instance->stop();
    // });

    instance->wait();
}
