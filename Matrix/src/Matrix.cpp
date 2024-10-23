#include "Matrix.hpp"

/*=============================================================================*/
/*==============================Конструкторы===================================*/
/*=============================================================================*/

Matrix::Matrix() : line(0), column(0), mtrx(nullptr)
{}
Matrix::Matrix(size_t initLine, size_t initColumn) : line(initLine), column(initColumn), mtrx(nullptr)
{
    if (initLine == 0 || initColumn == 0) 
    {
        throw std::invalid_argument("Matrix dimensions must be greater than zero.");
    }

    mtrx = new double* [initLine];

    for (int i = 0; i < initLine; ++i)
    {
        mtrx[i] = new double[initColumn];
    }

    for (size_t i = 0; i < initLine; ++i)
    {
        for (size_t j = 0; j < initColumn; ++j)
        {
            mtrx[i][j] = 0;
        }
    }
}
Matrix::Matrix(const Matrix& rhs)
{
    if (rhs.line == 0 || rhs.column == 0) 
    {
        throw std::invalid_argument("Cannot copy an empty matrix.");
    }

    this->column = rhs.column;
    this->line = rhs.line;
    this->mtrx = new double* [line];

    for (size_t i = 0; i < line; ++i)
    {
        mtrx[i] = new double[column];
    }

    for (size_t i = 0; i < line; ++i)
    {
        for (size_t j = 0; j < column; ++j)
        {
            mtrx[i][j] = rhs.mtrx[i][j];
        }
    }
}

// Конструктор, принимающий вектор векторов

Matrix::Matrix(const std::vector<std::vector<double>>& vec) 
{
    // Проверка, что вектор не пустой
    if (vec.empty() || vec[0].empty()) 
    {
        throw std::invalid_argument("Input vector is empty or invalid.");
    }

    // Определяем размеры матрицы на основе переданного вектора векторов
    line = vec.size();
    column = vec[0].size();

    // Выделяем память под матрицу
    mtrx = new double* [line];
    for (size_t i = 0; i < line; ++i)
    {
        mtrx[i] = new double[column];
    }

    // Проверка на правильность размеров векторов (все строки должны иметь одинаковую длину)
    for (size_t i = 0; i < line; ++i) 
    {
        if (vec[i].size() != column) 
        {
            throw std::invalid_argument("All rows in the input vector must have the same number of columns.");
        }
    }

    // Копируем данные из вектора в вектор
    for (size_t i = 0; i < line; ++i) 
    {
        for (size_t j = 0; j < column; ++j) 
        {
            mtrx[i][j] = vec[i][j];
        }
    }
}

Matrix::Matrix(const std::string& string) {
    std::vector<std::vector<double>> tempMatrix;
    std::string rowString;
    std::istringstream input(string);
    
    // Считываем каждую строку, заключённую в скобки
    while (std::getline(input, rowString, ')')) {
        size_t openBracket = rowString.find('(');
        if (openBracket != std::string::npos) {
            rowString = rowString.substr(openBracket + 1); // Убираем '('
            std::istringstream rowStream(rowString);
            std::vector<double> rowValues;
            double value;
            
            // Парсим значения через пробел
            while (rowStream >> value) {
                rowValues.push_back(value);
            }

            // Добавляем строку в матрицу
            if (!rowValues.empty()) {
                tempMatrix.push_back(rowValues);
            }
        }
    }

    // Проверка на корректность матрицы: все строки должны иметь одинаковое число столбцов
    size_t rows = tempMatrix.size();
    if (rows == 0) {
        throw std::invalid_argument("Matrix must have at least one row.");
    }

    size_t cols = tempMatrix[0].size();
    if (cols == 0) {
        throw std::invalid_argument("Matrix must have at least one column.");
    }

    for (const auto& row : tempMatrix) {
        if (row.size() != cols) {
            throw std::invalid_argument("All rows must have the same number of columns.");
        }
    }

    // Инициализируем матрицу
    line = rows;
    column = cols;
    mtrx = new double* [line];
    for (size_t i = 0; i < line; ++i) {
        mtrx[i] = new double[column];
        for (size_t j = 0; j < column; ++j) {
            mtrx[i][j] = tempMatrix[i][j];
        }
    }
}

Matrix::~Matrix()
{
    for (size_t i = 0; i < line; ++i)
    {
        delete[] mtrx[i];
    }
    delete[] mtrx;
}

/*=============================================================================*/
/*================= Рандомное заполнение матрицы из диапазона =================*/
/*=============================================================================*/
void Matrix::matrix_random(int32_t a, int32_t b)
{
    if (mtrx == nullptr) 
    {
        throw std::runtime_error("Matrix is not initialized.");
    }

    srand(time(NULL));
    for (uint32_t i = 0; i < this->line; ++i)
    {
        for (uint32_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] = rand()%(b - a + 1)+a;
        }
    }
}

/*=============================================================================*/
/*======================== Доступ к элементу по индексу =======================*/
/*=============================================================================*/
double* Matrix::operator[](const size_t& i)
{
    if (mtrx == nullptr) throw std::runtime_error("Matrix is not initialized.");
    if (i >= this->line) throw std::out_of_range("Index out of range.");
    return mtrx[i];
}



/*=============================================================================*/
/*============================Оператор присваивания ===========================*/
/*=============================================================================*/

Matrix& Matrix::operator=(const Matrix& rhs)
{
    if (this == &rhs) return *this;

    // Если размеры не совпадают, нужно пересоздать матрицу
    if (this->line != rhs.line || this->column != rhs.column) 
    {
        // Удаляем старую матрицу
        for (size_t i = 0; i < line; ++i) 
        {
            delete[] mtrx[i];
        }
        delete[] mtrx;

        // Создаем новую матрицу с нужным размером
        line = rhs.line;
        column = rhs.column;
        mtrx = new double* [line];
        for (size_t i = 0; i < line; ++i) 
        {
            mtrx[i] = new double[column];
        }
    }

    // Копируем данные
    for (size_t i = 0; i < line; ++i) 
    {
        for (size_t j = 0; j < column; ++j) 
        {
            mtrx[i][j] = rhs.mtrx[i][j];
        }
    }

    return *this;
}


/*=============================================================================*/
/*==========================Составные операторы присваивания===================*/
/*=============================================================================*/

Matrix Matrix::operator*=(const int32_t rhs)
{
    *this = (*this * rhs);
    return *this;
}
Matrix operator*=(const int32_t lhs, Matrix& rhs)
{
    rhs = lhs * rhs;
    return rhs;
}
Matrix Matrix::operator*=(const double rhs)
{
    *this = (*this * rhs);
    return *this;
}
Matrix operator*=(const double lhs, Matrix& rhs)
{
    rhs = lhs * rhs;
    return rhs;
}
Matrix Matrix::operator/=(const int32_t rhs)
{
    *this = (*this / rhs);
    return *this;
}
Matrix Matrix::operator/=(const double rhs)
{
    *this = (*this / rhs);
    return *this;
}
Matrix Matrix::operator+=(const Matrix& rhs)
{
    *this = *this + rhs;
    return *this;
}
Matrix Matrix::operator-=(const Matrix& rhs)
{
    *this = *this - rhs;
    return *this;
}

/*=============================================================================*/
/*========================Операторы умножения==================================*/
/*=============================================================================*/

Matrix Matrix::operator*(const int32_t rhs) const
{
    for (int32_t i = 0; i < this->line; ++i)
    {
        for (int32_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] *= rhs;
        }
    }
    return *this;
}
Matrix operator*(const int32_t lhs, const Matrix& rhs)
{
    return rhs * lhs;
}
Matrix Matrix::operator*(const double rhs) const
{
    for (int32_t i = 0; i < this->line; ++i)
    {
        for (int32_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] *= rhs;
        }
    }
    return *this;
}
Matrix operator*(const double lhs, const Matrix& rhs)
{
    return rhs * lhs;
}
Matrix Matrix::operator*(const Matrix& rhs) const
{

    if (this->column != rhs.line) throw std::runtime_error("The number of columns in the first factor is not equal to the number of rows in the second");
    Matrix result(this->line, rhs.column);
    for (size_t i = 0; i < result.line; ++i)
    {
        for (size_t j = 0; j < result.column; ++j)
        {
            result.mtrx[i][j] = 0;
            for (size_t k = 0; k < this->column; ++k)
            {
                result.mtrx[i][j] += this->mtrx[i][k] * rhs.mtrx[k][j];
            }
        }
    }
    // Округление малых значений в матрице
    for (size_t i = 0; i < result.line; ++i) 
    {
        for (size_t j = 0; j < result.column; ++j) 
        {
            if (std::abs(result.mtrx[i][j]) < 1e-9) {
                result.mtrx[i][j] = 0.0;
            }
        }
    }
    return result;
}

/*=============================================================================*/
/*========================Операторы деления====================================*/
/*=============================================================================*/

Matrix Matrix::operator/(const int32_t rhs) const
{
    if (rhs == 0) throw std::invalid_argument("Division by zero!!!");
    for (int32_t i = 0; i < this->line; ++i)
    {
        for (int32_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] /= rhs;
        }
    }
    return *this;
}
Matrix Matrix::operator/(const double rhs) const
{
    if (rhs == 0) throw std::invalid_argument("Division by zero!!!");
    for (int32_t i = 0; i < this->line; ++i)
    {
        for (int32_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] /= rhs;
        }
    }
    return *this;
}

/*=============================================================================*/
/*================Сравнение матриц на равенство и неравенство==================*/
/*=============================================================================*/
bool Matrix::operator==(const Matrix& rhs) const
{
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Matrix sizes do not match");
    for (size_t i = 0; i < rhs.line; ++i)
    {
        for (size_t j = 0; j < rhs.column; ++j)
        {
            if (this->mtrx[i][j] != rhs.mtrx[i][j]) return false;
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& rhs) const
{
    return !(*this == rhs);
}

/*=============================================================================*/
/*====================== Сложение матриц ======================================*/
/*=============================================================================*/

Matrix Matrix::operator+(const Matrix& rhs)
{
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Matrix sizes do not match");
    for (size_t i = 0; i < this->line; ++i)
    {
        for (size_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] += rhs.mtrx[i][j];
        }
    }
    return *this;
}

/*=============================================================================*/
/*====================== Вычитание матриц =====================================*/
/*=============================================================================*/

Matrix Matrix::operator-(const Matrix& rhs)
{
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Matrix sizes do not match");
    for (size_t i = 0; i < this->line; ++i)
    {
        for (size_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] -= rhs.mtrx[i][j];
        }
    }
    return *this;
}

/*================================================================================*/
/*=============== Нахождение определителя методом LU-разложения ==================*/
/*================================================================================*/
double Matrix::determinant() 
{
    // Проверяем, является ли матрица квадратной, так как определитель существует только для квадратных матриц
    if (this->column != this->line) throw std::runtime_error("Error when calculating the determinant! The matrix is ​​not square!!!");

    size_t n = this->line; // Размерность квадратной матрицы (n x n)
    Matrix L(n, n);  // Создаем нижнюю треугольную матрицу L (изначально заполняется нулями)
    Matrix U(*this);  // Верхняя треугольная матрица U (изначально копируем исходную матрицу)
    double det = 1.0; // Начальное значение для определителя
    int permutationCount = 0; // Счетчик перестановок строк для учёта изменения знака определителя

    // Проходим по всем столбцам для приведения матрицы к верхнетреугольному виду (метод Гаусса с выбором главного элемента)
    for (size_t i = 0; i < n; ++i)
    {
        // Находим максимальный элемент в текущем столбце ниже или на диагонали
        double maxElement = std::abs(U.mtrx[i][i]);
        size_t maxRow = i; // Индекс строки с максимальным элементом

        // Ищем строку с наибольшим элементом в текущем столбце для перестановки
        for (size_t k = i + 1; k < n; ++k) 
        {
            if (std::abs(U.mtrx[k][i]) > maxElement)
            {
                maxElement = std::abs(U.mtrx[k][i]);
                maxRow = k;
            }
        }

        // Если максимальный элемент равен нулю, то определитель матрицы равен 0 (матрица вырожденная)
        if (maxElement == 0.0) return det = 0;

        // Если строка с максимальным элементом не совпадает с текущей строкой, выполняем перестановку строк
        if (maxRow != i)
        {
            for (size_t k = 0; k < n; ++k)
            {
                std::swap(U.mtrx[maxRow][k], U.mtrx[i][k]); // Меняем строки в верхней матрице U
            }
            ++permutationCount;  // Увеличиваем счетчик перестановок
        }

        // Для каждой строки ниже текущей мы вычисляем коэффициент для зануления элементов ниже диагонали
        double factor;    // Коэффициент для зануления элементов ниже диагонали
        for (size_t j = i + 1; j < n; ++j)
        {
            factor = U.mtrx[j][i] / U.mtrx[i][i]; // Коэффициент для зануления
            L.mtrx[j][i] = factor; // Заполняем элемент в нижней треугольной матрице L

            // Модифицируем строку в матрице U, чтобы занулить элементы ниже главной диагонали
            for (size_t k = i; k < n; ++k)
            {
                U.mtrx[j][k] -= factor * U.mtrx[i][k]; // Вычитаем линейную комбинацию для зануления
            }
        }
    }

    // Теперь определитель равен произведению элементов на главной диагонали матрицы U
    for (size_t i = 0; i < n; ++i)
    {
        det *= U.mtrx[i][i]; // Умножаем элементы диагонали
    }

    // Если было нечетное количество перестановок строк, меняем знак определителя
    if (permutationCount % 2 != 0)
    {
        det = -det;
    }

    return det; // Возвращаем определитель матрицы
}


Matrix Matrix::inverse() 
{
    if (this->line != this->column)
        throw std::invalid_argument("Matrix must be square");

    size_t n = this->line;
    Matrix A(*this); 
    Matrix I(n, n); 

    

    for (size_t i = 0; i < n; ++i)
    {
        I.mtrx[i][i] = 1.0;
    }

    for (size_t i = 0; i < n; ++i)
    {
        double maxEl = std::abs(A.mtrx[i][i]);
        size_t maxRow = i;
        for (size_t k = i + 1; k < n; ++k) 
        {
            if (std::abs(A.mtrx[k][i]) > maxEl) 
            {
                maxEl = std::abs(A.mtrx[k][i]);
                maxRow = k;
            }
        }

        if (maxRow != i) 
        {
            for (size_t k = 0; k < n; ++k) 
            {
                std::swap(A.mtrx[maxRow][k], A.mtrx[i][k]);
                std::swap(I.mtrx[maxRow][k], I.mtrx[i][k]);
            }
        }

        if (A.mtrx[i][i] == 0.0) 
        {
            throw std::runtime_error("Matrix is singular and cannot be inverted");
        }

        double diagElement = A.mtrx[i][i];
        for (size_t k = 0; k < n; ++k) 
        {
            A.mtrx[i][k] /= diagElement;
            I.mtrx[i][k] /= diagElement;
        }

        for (size_t j = 0; j < n; ++j) 
        {
            if (j != i) 
            {
                double factor = A.mtrx[j][i];
                for (size_t k = 0; k < n; ++k) 
                {
                    A.mtrx[j][k] -= factor * A.mtrx[i][k];
                    I.mtrx[j][k] -= factor * I.mtrx[i][k];
                }
            }
        }
    }
    return I;
}

/*================================================================================*/
/*=========================== Транспонирование матрицы ===========================*/
/*================================================================================*/

void Matrix::transpose() 
{
    if (mtrx == nullptr) 
    {
        throw std::runtime_error("Cannot transpose an uninitialized matrix.");
    }

    double** transposedMtrx = new double* [column];

    for (size_t i = 0; i < column; ++i) 
    {
        transposedMtrx[i] = new double[line];
    }

    for (size_t i = 0; i < line; ++i) 
    {
        for (size_t j = 0; j < column; ++j) 
        {
            transposedMtrx[j][i] = mtrx[i][j];
        }
    }

    for (size_t i = 0; i < line; ++i) 
    {
        delete[] mtrx[i];
    }
    delete[] mtrx;

    std::swap(line, column);

    mtrx = transposedMtrx;
}

/*=============================================================================*/
/*========================Ввод и вывод матрицы=================================*/
/*=============================================================================*/

std::ostream& operator<<(std::ostream& out, const Matrix& rhs)
{
    out << '\n';
    for (size_t i = 0; i < rhs.line; ++i)
    {
        for (size_t j = 0; j < rhs.column; ++j)
        {
            out << rhs.mtrx[i][j] << '\t';
        }
        out << '\n';
    }
    return out;
}
std::istream& operator>>(std::istream& in, Matrix& obj)
{
    for (size_t i = 0; i < obj.line; ++i)
    {
        for (size_t j = 0; j < obj.column; ++j)
            in >> obj.mtrx[i][j];
    }
    return in;
}
