#include "rand.hpp"

namespace cosmo {

std::random_device              rand_seed_;
std::mt19937                    rand_gen_(rand_seed_());
std::uniform_int_distribution<> rand_spawn(-1000, 1000);

} // namespace cosmo
