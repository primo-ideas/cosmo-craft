// #include <chrono>
// #include <thread>

#include <gtest/gtest.h>

#include "lib.hpp"

TEST(Instance, Instantiation_And_Stop) {

    ASSERT_NO_THROW(auto instance = cosmo::Instance::launch(); instance.stop());
}
