#include "Random.h"

using namespace ONEngine;


std::mt19937 Random::generator_(std::random_device{}());
