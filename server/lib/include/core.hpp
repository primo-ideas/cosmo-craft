#pragma once

#include <expected>
#include <memory>
#include <string>
#include <system_error>

namespace cosmo {

class Instance {
  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Instance(Instance &&) noexcept;
    Instance &operator=(Instance &&) noexcept;
    ~Instance();

  private:
    explicit Instance(std::unique_ptr<Impl> impl);

  public:
    unsigned short port() const;
    void stop();
    void wait();

  public:
    [[nodiscard]]
    static std::expected<Instance, std::error_code>
    launch(unsigned short port = 0, std::string const &bind_addr = "127.0.0.1");
};

extern void init_logger();

} // namespace cosmo
