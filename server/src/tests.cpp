// #include <chrono>
// #include <thread>

#include <gtest/gtest.h>

#include "lib.hpp"

TEST(All, All) {

    ASSERT_NO_THROW(auto instance = cosmo::Instance::launch(); instance.stop());
}
