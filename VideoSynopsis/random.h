#pragma once

static unsigned int seed;

inline void rand_seed(unsigned int __seed)
{
	seed = __seed;
}

inline int fast_rand()
{
	unsigned int x = seed;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	seed = x;
	return x & 0x7fffffff;
}