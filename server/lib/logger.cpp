#include "logger.hpp"

#include "spdlog/pattern_formatter.h"

#ifdef _WIN32
#include <Windows.h>

#include <consoleapi.h>
#include <winbase.h>
#include <winnt.h>
#endif

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "core.hpp"
// #include "spdlog/common.h"
// #include "spdlog/sinks/wincolor_sink.h"

auto cosmo::logger = spdlog::logger("cosmo_logger");

namespace cosmo {

std::string thread_color(size_t thread_id) {
    static std::mutex mutex;
    static std::unordered_map<size_t, std::string> colors;

    std::lock_guard lock(mutex);
    if (auto it = colors.find(thread_id); it != colors.end())
        return it->second;

    constexpr double golden_ratio_conjugate = 0.618033988749895;
    double hue = std::fmod(colors.size() * golden_ratio_conjugate, 1.0) * 6;

    // HSV -> RGB avec S = 0.7 et V = 1
    constexpr double s = 0.7;
    double f = hue - std::floor(hue);
    double p = 1 - s, q = 1 - s * f, t = 1 - s * (1 - f);
    double r, g, b;
    switch (static_cast<int>(hue)) {
    case 0:
        r = 1, g = t, b = p;
        break;
    case 1:
        r = q, g = 1, b = p;
        break;
    case 2:
        r = p, g = 1, b = t;
        break;
    case 3:
        r = p, g = q, b = 1;
        break;
    case 4:
        r = t, g = p, b = 1;
        break;
    default:
        r = 1, g = p, b = q;
        break;
    }

    auto color = fmt::format("\033[38;2;{};{};{}m", std::lround(r * 255), std::lround(g * 255),
                             std::lround(b * 255));
    return colors.emplace(thread_id, std::move(color)).first->second;
}

class thread_flag : public spdlog::custom_flag_formatter {
    bool color_;

  public:
    explicit thread_flag(bool color)
        : color_(color) {
    }

    void format(spdlog::details::log_msg const &msg, std::tm const &,
                spdlog::memory_buf_t &dest) override {
        if (color_)
            fmt::format_to(std::back_inserter(dest), "{}[{}]\033[0m ", thread_color(msg.thread_id),
                           msg.thread_id);
        else
            fmt::format_to(std::back_inserter(dest), "[{}] ", msg.thread_id);
    }

    std::unique_ptr<custom_flag_formatter> clone() const override {
        return std::make_unique<thread_flag>(color_);
    }
};

void init_logger() {
#ifdef _WIN32
    // Active les séquences ANSI dans la console Windows.
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(out, &mode))
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
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
