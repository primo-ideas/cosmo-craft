#include "logger.hpp"

// #ifdef _WIN32
// #include <consoleapi2.h>
// #endif

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "core.hpp"
// #include "spdlog/common.h"
// #include "spdlog/sinks/wincolor_sink.h"

auto cosmo::logger = spdlog::logger("cosmo_logger");

namespace cosmo {

void init_logger() {
    spdlog::set_pattern("[%5t] [%H:%M:%S] %v");
    spdlog::cfg::load_env_levels();
    // #ifdef _WIN32
    //     auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
    //     console_sink->set_color(spdlog::level::trace, );
    // #else
    //     auto console_sink =
    //         std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>(spdlog::color_mode::automatic);
    //     console_sink->set_color(spdlog::level::trace, "\033[90m");
    // #endif
}

} // namespace cosmo
