#include <expected>
#include <ranges>
#include <system_error>
#include <tuple>
#include <utility>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "core.hpp"
#include "lib/game.hpp"

class TestClient {
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;

  private:
    TestClient(boost::beast::websocket::stream<boost::asio::ip::tcp::socket> &&ws)
        : ws_(std::move(ws)) {
    }

  public:
    TestClient(TestClient &&) = default;

    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> &ws() {
        return ws_;
    }

    std::expected<cosmo::AuthResponse, std::error_code> authenticate(std::string const &nickname) {
        auto write_str = cosmo::serialize(cosmo::ClientAuth{nickname});
        boost::system::error_code ec;
        ws_.write(boost::asio::buffer(write_str), ec);
        if (ec)
            return std::unexpected(ec);
        boost::beast::flat_buffer buffer;
        ws_.read(buffer, ec);
        if (ec)
            return std::unexpected(ec);
        auto read_str =
            std::string(static_cast<char const *>(buffer.cdata().data()), buffer.cdata().size());
        return cosmo::deserialize<cosmo::AuthResponse>(read_str).value();
    }

  public:
    static std::expected<TestClient, std::error_code> handshake(boost::asio::io_context &ioc,
                                                                unsigned short port) {
        auto resolver = boost::asio::ip::tcp::resolver{ioc};
        auto ws = boost::beast::websocket::stream<boost::asio::ip::tcp::socket>{ioc};

        auto host = std::string("127.0.0.1");
        auto port_str = std::to_string(port);
        auto ec = boost::beast::error_code();
        auto const results = resolver.resolve(host, port_str, ec);
        if (ec) {
            return std::unexpected(ec);
        }

        boost::asio::connect(ws.next_layer(), results, ec);
        if (ec) {
            return std::unexpected(ec);
        }

        host += ':' + port_str;

        ws.set_option(boost::beast::websocket::stream_base::decorator(
            [](boost::beast::websocket::request_type &req) {
                req.set(boost::beast::http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) + " CosmoCraft");
            }));

        ws.handshake(host, "/", ec);
        if (ec) {
            return std::unexpected(ec);
        }
        return TestClient(std::move(ws));
    }
};

TEST(Instance, Nominal) {
    ASSERT_NO_THROW(std::ignore = cosmo::Instance::launch());
    ASSERT_NO_THROW(std::ignore = cosmo::Instance::launch());
    auto maybe_instance = cosmo::Instance::launch();
    ASSERT_TRUE(maybe_instance) << maybe_instance.error().message();
    auto instance = std::move(maybe_instance.value());
    ASSERT_NE(instance.port(), 0);
    ASSERT_NO_THROW(instance.stop());
    ASSERT_NO_THROW(instance.wait());
    maybe_instance = cosmo::Instance::launch(8080);
    ASSERT_TRUE(maybe_instance) << maybe_instance.error().message();
    instance = std::move(maybe_instance.value());
    ASSERT_EQ(instance.port(), 8080);
    ASSERT_NO_THROW(instance.stop());
    ASSERT_NO_THROW(instance.wait());
}

TEST(Instance, Handshake) {
    auto instance = cosmo::Instance::launch().value();
    auto ioc = boost::asio::io_context();
    auto maybe_client = TestClient::handshake(ioc, instance.port());
    ASSERT_TRUE(maybe_client) << maybe_client.error().message();
}

TEST(Instance, HundredHandshakes) {
    auto instance = cosmo::Instance::launch().value();
    auto ioc = boost::asio::io_context();
    auto clients = std::vector<TestClient>();
    for (auto _ : std::ranges::views::iota(1, 100)) {
        auto maybe_client = TestClient::handshake(ioc, instance.port());
        ASSERT_TRUE(maybe_client) << maybe_client.error().message();
        clients.emplace_back(std::move(maybe_client.value()));
    }
}

TEST(Instance, SimpleAuthentication) {
    auto instance = cosmo::Instance::launch().value();
    auto ioc = boost::asio::io_context();
    auto client1 = TestClient::handshake(ioc, instance.port()).value();
    auto maybe_response = client1.authenticate("Player1");
    ASSERT_TRUE(maybe_response) << maybe_response.error();
    auto response = maybe_response.value();
    ASSERT_TRUE(response.result);
}

TEST(Instance, DoubleAuthentication) {
    auto instance = cosmo::Instance::launch().value();
    auto ioc = boost::asio::io_context();
    auto client1 = TestClient::handshake(ioc, instance.port()).value();
    auto maybe_response = client1.authenticate("Player1");
    ASSERT_TRUE(maybe_response) << maybe_response.error();
    auto response = maybe_response.value();
    ASSERT_TRUE(response.result);
    auto client2 = TestClient::handshake(ioc, instance.port()).value();
    maybe_response = client2.authenticate("Player2");
    ASSERT_TRUE(maybe_response) << maybe_response.error();
    response = maybe_response.value();
    ASSERT_TRUE(response.result);
}

TEST(Instance, DoubleAuthenticationSameNickname) {
    auto instance = cosmo::Instance::launch().value();
    auto ioc = boost::asio::io_context();
    auto client1 = TestClient::handshake(ioc, instance.port()).value();
    auto maybe_response = client1.authenticate("Player1");
    ASSERT_TRUE(maybe_response) << maybe_response.error();
    auto response = maybe_response.value();
    ASSERT_TRUE(response.result);
    auto client2 = TestClient::handshake(ioc, instance.port()).value();
    maybe_response = client2.authenticate("Player1");
    ASSERT_TRUE(maybe_response) << maybe_response.error();
    response = maybe_response.value();
    ASSERT_FALSE(response.result);
}

int main(int argc, char **argv) {
    cosmo::init_logger();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
