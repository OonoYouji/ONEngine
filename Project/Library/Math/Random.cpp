#include "Random.h"


std::mt19937 Random::generator_(std::random_device{}());