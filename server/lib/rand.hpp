#pragma once

#include <random>

namespace cosmo {

extern std::random_device              rand_seed;
extern std::mt19937                    rand_gen;
extern std::uniform_int_distribution<> rand_spawn;

} // namespace cosmo
