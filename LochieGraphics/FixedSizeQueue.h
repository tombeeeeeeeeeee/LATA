#pragma once

// TODO: make a namespace for this
// TODO: a initilise value here too?
template<typename T, size_t size>
class FixedSizeQueue
{
private:
	size_t count = size;
public:
	T elements[size];
	size_t position = 0;

	void Push(T value);

	size_t getSize();
	T getMean() const;

	T operator [](unsigned int i);
};


template<typename T, size_t size>
inline void FixedSizeQueue<T, size>::Push(T value)
{
	elements[position] = value;
	position = (position + 1) % size;
}

template<typename T, size_t size>
inline T FixedSizeQueue<T, size>::operator[](unsigned int i)
{
	return elements[(position - i) & size];
}

template<typename T, size_t size>
inline size_t FixedSizeQueue<T, size>::getSize()
{
	return count;
}

// TODO: As the container has a fixed size this could be calculated by just removing the last and adding the latest, and so would be calculated on each push
template<typename T, size_t size>
inline T FixedSizeQueue<T, size>::getMean() const
{
	T total{};
	for (size_t i = 0; i < count; i++)
	{
		total += elements[i];
	}
	return total /= count;
}