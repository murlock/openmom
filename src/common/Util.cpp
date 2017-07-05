#include "common/Util.h"

std::mt19937 Util::rng(std::mt19937::default_seed);
std::uniform_real_distribution<float> Util::real(0.0f,1.0f);
std::uniform_int_distribution<> Util::integer(0, std::numeric_limits<int>::max());

const PositionOffset Util::DIRS[];
const PositionOffset Util::ODIRS[];
