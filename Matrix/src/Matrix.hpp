#ifndef MATRIX_H
#define MATRIX_H
#include <iostream> // Для ввода-Вывода
#include <ctime>	// Для srand(time(NULL))
#include <vector>	// Для поиска обратной матрицы
#include <stdexcept>// Для выброса стандартных исключений
#include <sstream>  // Для получения матрицы из строки вида (1 2 3)(4 5 6)(7 8 9)
class Matrix
{
private:
	size_t line;
	size_t column;
	double** mtrx;

public:
	// Конструктор по умолчанию
	Matrix();
	// Конструктор с параметрами
	Matrix(size_t, size_t);
	// Конструктор копирования
	Matrix(const Matrix&); 
	// Конструктор, принимающий вектор векторов
	Matrix(const std::vector<std::vector<double>>&);
	// Конструктор, принимающий строку
	Matrix(const std::string&);
	// Деструктор
	~Matrix();

	// Рандомное заполнение матрицы из диапазона
	void matrix_random(int32_t, int32_t);

	// Доступ к элементу по индексу
	double* operator[](const size_t&);

	// Оператор присваивания
	Matrix& operator=(const Matrix&);

	// Составные операторы присваивания
	Matrix operator*=(const int32_t);
	friend Matrix operator*=(const int32_t, Matrix&);
	Matrix operator*=(const double);
	friend Matrix operator*=(const double, Matrix&);
	Matrix operator/=(const int32_t);
	Matrix operator/=(const double);
	Matrix operator+=(const Matrix&);
	Matrix operator-=(const Matrix&);

	// Операторы умножения
	Matrix operator*(const int32_t) const;
	friend Matrix operator*(const int32_t, const Matrix&);
	Matrix operator*(const double) const;
	friend Matrix operator*(const double, const Matrix&);
	Matrix operator*(const Matrix&) const;

	// Операторы деления матрицы на число
	Matrix operator/(const int32_t) const;
	Matrix operator/(const double) const;

	// Сравнение матриц на равенство и неравенство
	bool operator==(const Matrix&) const;
	bool operator!=(const Matrix&) const;

	// Сложение матриц
	Matrix operator+(const Matrix&);

	// Вычитание матриц
	Matrix operator-(const Matrix&);
	
	// Нахождение определителя методом LU-раззложения
	double determinant();
	
	// Нахождение оброатной матрицы
	Matrix inverse();

	// Транспонирование матрицы
	void transpose();

	// Ввод и вывод матрицы
	friend std::ostream& operator<<(std::ostream&, const Matrix&);
	friend std::istream& operator>>(std::istream&, Matrix&);

};


#endif // !MATRIX_H
