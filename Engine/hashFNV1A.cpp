#include "hashFNV1A.h"

const unsigned long long hashFNV1A::offset = 14695981039346656037;
const unsigned long long hashFNV1A::prime = 1099511628211;


unsigned long long hashFNV1A::operator()(unsigned long long key) const
{
	unsigned long long hash = offset;
	hash ^= key;
	hash *= prime;
	return hash;
}

unsigned long long hashFNV1A::operator()(std::string key) const
{
	unsigned long long hash = offset;
	for (auto i = 0; i < key.size(); i++)
	{
		hash ^= key[i];
		hash *= prime;
	}
	return hash;
}

unsigned long long hashFNV1A::operator()(std::pair<int, int> key) const
{
	unsigned long long hash = offset;
	hash ^= key.first;
	hash *= prime;
	hash ^= key.second;
	hash *= prime;
	return hash;
}