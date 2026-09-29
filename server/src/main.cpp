#include <iostream>

#include <boost/program_options.hpp>

#include "lib.hpp"

namespace po = boost::program_options;
using namespace std;

int main(int ac, char **av) {
    po::options_description desc("Usage: cosmocraft-server");
    desc.add_options()("help", "show this help")("bind_addr", po::value<std::string>())(
        "port", po::value<unsigned short>());

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("compression")) {
        cout << "Compression level was set to " << vm["compression"].as<int>() << ".\n";
    } else {
        cout << "Compression level was not set.\n";
    }
    auto instance =
        cosmo::Instance::launch(vm["bind_addr"].as<std::string>(), vm["port"].as<unsigned short>());
}
