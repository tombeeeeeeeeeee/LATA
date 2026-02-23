#pragma once

#include <string>

struct hashFNV1A {
	static const unsigned long long offset;
	static const unsigned long long prime;
	unsigned long long operator()(unsigned long long key) const;
	unsigned long long operator()(std::string key) const;
	unsigned long long operator()(std::pair<int, int> key) const;
};
