#pragma once

#include <memory>
#include <string>

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
    static Instance launch(std::string const &bind_addr, unsigned short port);
    [[nodiscard]]
    static Instance launch();
};

} // namespace cosmo
