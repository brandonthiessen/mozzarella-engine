#ifndef __PERFT_H__
#define __PERFT_H__

#include "position.h"
#include "move_generation.h"

#include <cstdint>
#include <iostream>
#include <cassert>
#include <string>

uint64_t perft(Position *p, int depth);

#endif
