#include "BoolMatrix.hpp"

/*================================================================================*/
/*================================= Конструкторы =================================*/
/*================================================================================*/

BoolMatrix::BoolMatrix() : b_matrix(NULL), m_size(0) {}

BoolMatrix::BoolMatrix(int32_t init_n)
{
    if (init_n <= 0) throw InvalidKey("Размерность матрицы не может быть отрицательной или нулевой!!!");

    m_size = init_n;
    std::vector<std::vector<bool>> tempMatrix(m_size, std::vector<bool> (m_size, 0));

    b_matrix = tempMatrix;
}

BoolMatrix::BoolMatrix(std::vector<std::vector<bool>>& init_matrix) : b_matrix(init_matrix) {}

BoolMatrix::BoolMatrix(BoolMatrix& rhs) : b_matrix(rhs.b_matrix) {}

BoolMatrix::BoolMatrix(const std::string& str)   
{
    uint32_t size = uint32_t(sqrt(str.length()));

    if (size*size != str.length()) throw InvalidKey("Неверный размер строки. Длина строки должна быть равна квадрату числа!!!");
    std::vector<std::vector<bool>> tempMatrix(size, std::vector<bool>(size, 0));

    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            tempMatrix[i][j] = (str[(i*size) + j] == '0' ? 0 : 1);
        }
    }
    m_size = size;
    b_matrix = tempMatrix;
}

BoolMatrix::BoolMatrix(const std::wstring & str)
{
    uint32_t size = uint32_t(sqrt(str.length()));

    if (size*size != str.length()) throw InvalidKey("Неверный размер строки. Длина строки должна быть равна квадрату числа!!!");
    std::vector<std::vector<bool>> tempMatrix(size, std::vector<bool>(size, 0));

    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            tempMatrix[i][j] = (str[(i*size) + j] == '0' ? 0 : 1);
        }
    }
    m_size = size;
    b_matrix = tempMatrix;
}

BoolMatrix::~BoolMatrix() {}

/*================================================================================*/
/*=============================== Функции и методы ===============================*/
/*================================================================================*/

uint32_t BoolMatrix::size() {return this->m_size;}

void BoolMatrix::check()
{
    size_t holeCount = 0;
    
    // Проверяем только первую четверть матрицы
    for (size_t i = 0; i < m_size/2; ++i)
    {
        for (size_t j = 0; j < m_size/2; ++j)
        {
            if (b_matrix[i][j] == true)
            {                
                if (b_matrix[j][m_size-i-1] || b_matrix[m_size-i-1][m_size-j-1] || b_matrix[m_size-j-1][i]) 
                {
                    throw InvalidKey("Обнаружены пересечения в решетке Кардано!!!");
                }
            }
        }
    }
    
    /// Дополнительная проверка общего количества вырезов
    size_t totalHoles = 0;
    for (size_t i = 0; i < m_size; ++i) {
        for (size_t j = 0; j < m_size; ++j) {
            if (b_matrix[i][j]) totalHoles++;
        }
    }

    size_t expectedTotalHoles = (m_size * m_size) / 4;
    if (totalHoles != expectedTotalHoles) {
        throw std::invalid_argument("Общее количество вырезов в решетке должно быть " +  std::to_string(expectedTotalHoles) + "!");
    }
}

void BoolMatrix::rotation()
{
    BoolMatrix temp(this->m_size);

    for (size_t i = 0; i < this->m_size; ++i)
    {
        for (size_t j = 0; j < this->m_size; ++j)
        {
            temp.b_matrix[j][(this->m_size-1)-i] = this->b_matrix[i][j];
        }
    }
    for (size_t i = 0; i < m_size; ++i) 
    {
        for (size_t j = 0; j < m_size; ++j) 
        {
            this->b_matrix[i][j] = temp.b_matrix[i][j];
        }
    }
}

std::wstring BoolMatrix::decryption(const std::wstring& str)
{
    if (str.length() % (m_size * m_size) != 0)  throw InvalidOpenText("Длина текста не кратна квадрату стороны матрицы!!!");

    size_t blockCount = str.length() / (m_size * m_size);
    std::wstring resultString;
    std::wstring temp;

    for (size_t c = 0; c < blockCount; ++c)
    {
        temp.clear();
        for (uint16_t k = 0; k < 4; ++k)
        {
            for (size_t i = 0; i < m_size; ++i)
            {
                for (size_t j = 0; j < m_size; ++j)
                {
                    if (this->b_matrix[i][j]==1) temp.push_back(str[c * m_size * m_size + (i * m_size + j)]);
                }
            }
            this->rotation();
        }

        resultString.append(temp);
    }

    return resultString;
}


std::wstring BoolMatrix::encryption(const std::wstring& str)
{
    std::wstring string = str;
    size_t textSize = m_size * m_size;
    
    std::cout << "M_size: " << m_size << std::endl; 
    std::cout << "Text size: " << string.length() << std::endl;
    
    // Если длина не кратна textSize, дополняем текст его же началом
    //if (string.length() % textSize != 0) {
    //    size_t pad_length = textSize - (string.length() % textSize);
    //    string.append(string.substr(0, pad_length)); 
    //}
    
    if (string.length() % textSize != 0) {
        throw InvalidOpenText("Длина текста не кратна квадрату стороны матрицы!!!");
    }
    
    size_t count = string.length() / textSize;
    std::wstring resultString;
    
    for (size_t c = 0; c < count; ++c)
    {
        std::wstring block(textSize, L' '); // Инициализируем новый блок пробелами
        size_t tempPosition = 0;

        for (uint16_t k = 0; k < 4; ++k)
        {
            for (size_t i = 0; i < this->m_size; ++i)
            {
                for (size_t j = 0; j < this->m_size; ++j)
                {
                    if (this->b_matrix[i][j] == 1) 
                    { 
                        if (tempPosition < textSize) {
                            block[i * m_size + j] = string[c * textSize + tempPosition];
                            ++tempPosition;
                        }
                    }
                }
            }
            this->rotation();
        }

        resultString.append(block);
    }
    return resultString;
}


/*================================================================================*/
/*============================= Перегрузка операторов ============================*/
/*================================================================================*/

std::vector<bool>& BoolMatrix::operator[](int32_t i)
{
    if (i < 0 || i >= static_cast<int32_t>(m_size)) throw std::invalid_argument("Неверный индекс в операторе []!!!\n");
    return b_matrix[i];
}

std::ostream& operator<<(std::ostream& out, const BoolMatrix& obj)
{
    for (size_t i = 0; i < obj.m_size; ++i)
    {
        for (size_t j = 0; j < obj.m_size; ++j)
        {
            out << obj.b_matrix[i][j];
        }
    }
    return out;
}

std::istream& operator>>(std::istream& in, BoolMatrix& obj)
{
    int32_t temp;
    for (size_t i = 0; i < obj.m_size; ++i)
    {
        for (size_t j = 0; j < obj.m_size; ++j)
        {
            in >> temp;
            temp = (temp == 0? 0 : 1);
            obj.b_matrix[i][j] = temp;       
        }
    }
    return in;
}