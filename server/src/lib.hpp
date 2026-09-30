#pragma once

#include <memory>
#include <string>

namespace cosmo {

class Instance {
  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    ~Instance();

  private:
    explicit Instance(Impl &&impl);

  public:
    unsigned short port() const;
    void stop();
    void wait();

  public:
    static Instance launch(std::string const &bind_addr, unsigned short port);
    static Instance launch();
};

} // namespace cosmo
