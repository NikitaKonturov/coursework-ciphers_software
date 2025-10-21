#include "Matrix.hpp"


int64_t mod_inverse(int64_t a, int64_t mod) {
    int64_t m0 = mod, t, q;
    int64_t x0 = 0, x1 = 1;
    
    if (mod == 1) return 0;
    
    while (a > 1) {
        q = a / mod;
        t = mod;
        mod = a % mod;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    
    return (x1 < 0) ? x1 + m0 : x1;
}

/*=============================================================================*/
/*==============================Конструкторы===================================*/
/*=============================================================================*/

Matrix::Matrix() : line(0), column(0), mtrx(nullptr)
{}

Matrix::Matrix(size_t initLine, size_t initColumn, uint32_t mod) : line(initLine), column(initColumn), mtrx(nullptr), mod(mod)
{
    if (initLine == 0 || initColumn == 0) 
    {
        throw std::invalid_argument("Размеры матрицы должны быть больше нуля.");
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
        throw std::invalid_argument("Не удается скопировать пустую матрицу.");
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
        throw std::invalid_argument("Входной вектор пуст или недопустим.");
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
            throw std::invalid_argument("Все строки во входном векторе должны содержать одинаковое количество столбцов.");
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

Matrix::Matrix(const std::vector<std::vector<int64_t>>& vec) {
     // Проверка, что вектор не пустой
     if (vec.empty() || vec[0].empty()) 
     {
         throw std::invalid_argument("Входной вектор пуст или недопустим.");
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
             throw std::invalid_argument("Все строки во входном векторе должны содержать одинаковое количество столбцов.");
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
    while (std::getline(input, rowString, ']')) {
        size_t openBracket = rowString.find('[');
        if (openBracket != std::string::npos) {
            rowString = rowString.substr(openBracket + 1); // Убираем '['
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
        throw std::invalid_argument("Матрица должна содержать хотя бы одну строку.");
    }

    size_t cols = tempMatrix[0].size();
    if (cols == 0) {
        throw std::invalid_argument("Матрица должна содержать хотя бы один столбец.");
    }

    for (const auto& row : tempMatrix) {
        if (row.size() != cols) {
            throw std::invalid_argument("Все строки должны содержать одинаковое количество столбцов.");
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

Matrix::Matrix(const std::wstring & string)
{
    std::vector<std::vector<double>> tempMatrix;
    std::wstring rowString;
    std::wistringstream input(string);
    
    // Считываем каждую строку, заключённую в скобки
    while (std::getline(input, rowString, L']')) {
        size_t openBracket = rowString.find(L'[');
        if (openBracket != std::string::npos) {
            rowString = rowString.substr(openBracket + 1); // Убираем '['
            std::wistringstream rowStream(rowString);
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
        throw std::invalid_argument("Матрица должна содержать хотя бы одну строку.");
    }

    size_t cols = tempMatrix[0].size();
    if (cols == 0) {
        throw std::invalid_argument("Матрица должна содержать хотя бы один столбец.");
    }

    for (const auto& row : tempMatrix) {
        if (row.size() != cols) {
            throw std::invalid_argument("Все строки должны содержать одинаковое количество столбцов.");
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


void Matrix::matrix_random(int64_t a, int64_t b, int64_t mod) {
    if (mtrx == nullptr) {
        throw std::runtime_error("Матрица не инициализирована.");
    }

    HMAC_DRBG gen(get_entropy(), get_entropy());

    while (true) {
        // Генерируем случайную матрицу
        for (int64_t i = 0; i < this->line; ++i) {
            for (int64_t j = 0; j < this->column; ++j) {
                if (gen.HMAC_DRBG_Ressed_Check()) {
                    gen.HMAC_DRBG_Ressed(get_entropy());
                }
                this->mtrx[i][j] = convert_bytes_to_ddword(gen.HMAC_DRBG_Generate_algorithm(9).value()) % mod;
            }
        }

        // Вычисляем определитель
        int64_t det = this->determinant();

        // Проверяем условия: det ≠ 0 и НОД(det, mod) == 1
        if (det != 0 && std::gcd(det, mod) == 1) {
            break; // Матрица подходит, выходим из цикла
        }
    }
}


/*=============================================================================*/
/*======================== Доступ к элементу по индексу =======================*/
/*=============================================================================*/
double*& Matrix::operator[](const size_t& i)
{
    if (mtrx == nullptr) throw std::runtime_error("Матрица не инициализирована.");
    if (i >= this->line) throw std::out_of_range("Индекс выходит за пределы допустимого диапазона.");
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

size_t Matrix::get_lines() const
{
    return this->line;
}

size_t Matrix::get_columns() const
{
    return this->column;
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

void Matrix::find_and_swap_rows(uint32_t inull, uint32_t jnull, Matrix &source) {
    for (size_t i = inull + 1; i < source.line; ++i) {
        if(static_cast<uint32_t>(source[i][jnull]) != 0) {
            std::swap(source[i], source[inull]);
            break;
        }      
    }
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

Matrix Matrix::operator*(const Matrix& rhs) const {
    if (this->column != rhs.line) 
        throw std::runtime_error("Количество столбцов в первом множителе не равно количеству строк во втором");

    Matrix result(this->line, rhs.column, this->mod);
    int64_t mod = this->mod;

    for (size_t i = 0; i < result.line; ++i) {
        for (size_t j = 0; j < result.column; ++j) {
            int64_t sum = 0;
            for (size_t k = 0; k < this->column; ++k) {
                sum = (sum + static_cast<int64_t>(static_cast<int64_t>(this->mtrx[i][k]) * static_cast<int64_t>(rhs.mtrx[k][j])) % mod) % mod;
            }
            if (sum < 0) sum += mod;
            result.mtrx[i][j] = sum;
        }
    }
    return result;
}




/*=============================================================================*/
/*========================Операторы деления====================================*/
/*=============================================================================*/

Matrix Matrix::operator/(const int32_t rhs) const
{
    if (rhs == 0) throw std::invalid_argument("Деление на ноль!!!");
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
    if (rhs == 0) throw std::invalid_argument("Деление на ноль!!!");
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
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Размеры матриц не совпадают");
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
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Размеры матриц не совпадают");
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
    if (this->column != rhs.column || this->line != rhs.line) throw std::invalid_argument("Размеры матриц не совпадают");
    for (size_t i = 0; i < this->line; ++i)
    {
        for (size_t j = 0; j < this->column; ++j)
        {
            this->mtrx[i][j] -= rhs.mtrx[i][j];
        }
    }
    return *this;
}

void reduce_global_common_factor(double* arr1, double* arr2, size_t n) {
    while (true) {
        int64_t global_gcd = arr1[0];
        for (size_t i = 1; i < n; ++i)
            global_gcd = std::gcd(global_gcd, static_cast<int64_t>(arr1[i]));

        for (size_t i = 0; i < n; ++i)
            global_gcd = std::gcd(global_gcd, static_cast<int64_t>(arr2[i]));

        if (global_gcd == 1) break; // Остановка, если больше нечего сокращать

        for (size_t i = 0; i < n; ++i) {
            arr1[i] = static_cast<int64_t>(arr1[i])  / global_gcd;
            arr2[i] = static_cast<int64_t>(arr2[i]) / global_gcd;
        }
    }
}

/*================================================================================*/
/*=============== Нахождение определителя методом LU-разложения ==================*/
/*================================================================================*/
int64_t Matrix::determinant() {
    if (this->column != this->line) {
        throw std::runtime_error("Ошибка: Матрица должна быть квадратной для вычисления определителя.");
    }

    size_t n = this->line;
    Matrix temp(*this); // Создаём копию матрицы, чтобы не менять исходную
    int64_t det = 1;

    for (size_t i = 0; i < n; ++i) {
        // Ищем ненулевой ведущий элемент
        size_t pivot = i;
        while (pivot < n && temp.mtrx[pivot][i] == 0) {
            ++pivot;
        }

        // Если не нашли ненулевой элемент, определитель равен 0
        if (pivot == n) return 0;

        // Если строка сменилась, меняем знак определителя
        if (pivot != i) {
            std::swap(temp.mtrx[i], temp.mtrx[pivot]);
            det = (-det + mod) % mod;
        }

        int64_t pivotValue = temp.mtrx[i][i];

        // Если ведущий элемент не обратим, определитель 0
        if (std::gcd(pivotValue, mod) != 1) return 0;

        det = (det * pivotValue) % mod;

        // Прямой ход метода Гаусса
        for (size_t j = i + 1; j < n; ++j) {
            if (temp.mtrx[j][i] == 0) continue;

            int64_t factor = static_cast<int64_t>(temp.mtrx[j][i] * mod_inverse(pivotValue, mod)) % mod;

            for (size_t k = i; k < n; ++k) {
                temp.mtrx[j][k] = static_cast<int64_t>(temp.mtrx[j][k] - factor * temp.mtrx[i][k]) % mod;
                if (temp.mtrx[j][k] < 0) temp.mtrx[j][k] += mod;
            }
        }
    }

    return det;
}


// Гауссов метод нахождения обратной матрицы в Z_n
Matrix inverse_matrix_gauss(Matrix A, int64_t mod) {
    int64_t n = A.get_lines();
    Matrix I(n, n, mod);

    // Формируем единичную матрицу
    for (int64_t i = 0; i < n; i++) {
        I[i][i] = 1;
    }

    // Прямой ход метода Гаусса
    for (int64_t i = 0; i < n; i++) {
        if (A[i][i] == 0) {
            // Ищем строку с ненулевым элементом в этом столбце
            bool swapped = false;
            for (int64_t k = i + 1; k < n; k++) {
                if (A[k][i] != 0) {
                    std::swap(A[i], A[k]);
                    std::swap(I[i], I[k]);
                    swapped = true;
                    break;
                }
            }
            if (!swapped) throw std::runtime_error("Матрица необратима в Z_n");
        }

        if(static_cast<int64_t>(A[i][i]) < 0) {
            A[i][i] = (static_cast<int64_t>(A[i][i]) % mod + mod) % mod;
        }
        int64_t invElem = mod_inverse(static_cast<int64_t>(A[i][i]), mod);
        std::cout << "mod_inverse(" << static_cast<int64_t>(A[i][i]) << ", " << mod << ") = " << mod_inverse(static_cast<int64_t>(A[i][i]), mod) << std::endl;
        if (invElem == 0) throw std::runtime_error("Не существует обратного элемента");

        // Нормируем строку
        for (int64_t j = 0; j < n; j++) {
            A[i][j] = static_cast<int64_t>(A[i][j] * invElem) % mod;
            I[i][j] = static_cast<int64_t>(I[i][j] * invElem) % mod;
        }

        // Обнуляем элементы под главной диагональю
        for (int64_t k = i + 1; k < n; k++) {
            int64_t factor = A[k][i];
            for (int64_t j = 0; j < n; j++) {
                A[k][j] = static_cast<int64_t>(A[k][j] - factor * A[i][j] + mod) % mod;
                I[k][j] = static_cast<int64_t>(I[k][j] - factor * I[i][j] + mod) % mod;
            }
        }
    }

    // Обратный ход метода Гаусса
    for (int64_t i = n - 1; i >= 0; i--) {
        for (int64_t k = i - 1; k >= 0; k--) {
            int64_t factor = A[k][i];
            for (int64_t j = 0; j < n; j++) {
                A[k][j] = static_cast<int64_t>(A[k][j] - factor * A[i][j] + mod) % mod;
                I[k][j] = static_cast<int64_t>(I[k][j] - factor * I[i][j] + mod) % mod;
            }
        }
    }

    return I;
}


Matrix Matrix::inverse(int64_t mod) {
    if (this->line != this->column)
        throw std::invalid_argument("Матрица должна быть квадратной");

    Matrix res = inverse_matrix_gauss(*this, mod);
    
    for (size_t i = 0; i < res.line; ++i) {
        for (size_t j = 0; j < res.column; ++j) {
            res[i][j] = static_cast<int64_t>(static_cast<int64_t>(res[i][j]) % mod + mod) % mod; 
        }
    }
     
    return  res;
}
    

/*================================================================================*/
/*=========================== Транспонирование матрицы ===========================*/
/*================================================================================*/

void Matrix::transpose() 
{
    if (mtrx == nullptr) 
    {
        throw std::runtime_error("Невозможно транспонировать неинициализированную матрицу.");
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
        out << "[";
        for (size_t j = 0; j < rhs.column; ++j)
        {
            out << rhs.mtrx[i][j] << ' ';
        }
        out << "]\n";
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
