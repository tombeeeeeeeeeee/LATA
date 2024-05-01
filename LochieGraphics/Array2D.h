#pragma once

#include <vector>

template<typename T, int _rows, int _cols>
class Array2D // TODO: Named array but really using a vector
{
private:
	int rows = _rows;
	int cols = _cols;

	std::vector<T> data;

	int getIndex(int row, int col) const;
public:

	Array2D();
	T& operator ()(int row, int col);


	int getRowCount() const;
	int getColCount() const;
	int getSize() const;

	typename std::vector<T>::iterator Begin();
	typename std::vector<T>::iterator End();
};

template<typename T, int _rows, int _cols>
inline int Array2D<T, _rows, _cols>::getIndex(int row, int col) const
{
	return row * cols + col;
}

template<typename T, int _rows, int _cols>
inline Array2D<T, _rows, _cols>::Array2D()
{
	data.resize(getSize());
}

template<typename T, int _rows, int _cols>
inline T& Array2D<T, _rows, _cols>::operator()(int row, int col)
{
	row = (row % rows + rows) % rows;
	col = (col % cols + cols) % cols;


	return data[getIndex(row, col)];
}

template<typename T, int _rows, int _cols>
inline int Array2D<T, _rows, _cols>::getRowCount() const
{
	return rows;
}

template<typename T, int _rows, int _cols>
inline int Array2D<T, _rows, _cols>::getColCount() const
{
	return cols;
}

template<typename T, int _rows, int _cols>
inline int Array2D<T, _rows, _cols>::getSize() const
{
	return rows * cols;
}

template<typename T, int _rows, int _cols>
inline typename std::vector<T>::iterator Array2D<T, _rows, _cols>::Begin()
{
	return data.begin();
}

template<typename T, int _rows, int _cols>
inline typename std::vector<T>::iterator Array2D<T, _rows, _cols>::End()
{
	return data.end();
}
