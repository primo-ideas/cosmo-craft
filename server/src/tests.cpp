#include <expected>
#include <system_error>
#include <tuple>

#include <gtest/gtest.h>

#include "lib.hpp"

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
}

TEST(Instance, Launch_Nominal) {
    ASSERT_NO_THROW(auto instance = cosmo::Instance::launch());
}

TEST(Instance, Launch_Custom_Port) {
    ASSERT_NO_THROW(auto instance = cosmo::Instance::launch());
}
