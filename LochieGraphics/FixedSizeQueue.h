#pragma once

// TODO: make a namespace for this
// TODO: a initilise value here too?
template<typename T, size_t size>
class FixedSizeQueue
{
public:
	T elements[size];
	size_t position = 0;

	void Push(T value);

	T operator [](unsigned int i);
};


template<typename T, size_t size>
void FixedSizeQueue<T, size>::Push(T value)
{
	position = (position + 1) % size;
	elements[position] = value;
}

template<typename T, size_t size>
inline T FixedSizeQueue<T, size>::operator[](unsigned int i)
{
	return elements[(position - i) & size];
}