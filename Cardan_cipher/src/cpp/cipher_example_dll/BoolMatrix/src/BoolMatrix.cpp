#include "BoolMatrix.hpp"

/*================================================================================*/
/*================================= Конструкторы =================================*/
/*================================================================================*/

BoolMatrix::BoolMatrix() : b_matrix(NULL), m_size(0) {}

BoolMatrix::BoolMatrix(int32_t init_n)
{
    if (init_n <= 0) throw InvalidKey("Matrix dimension cannot be negative or zero!!!");

    m_size = 2*init_n;
    std::vector<std::vector<bool>> tempMatrix(m_size, std::vector<bool> (m_size, 0));

    b_matrix = tempMatrix;
}

BoolMatrix::BoolMatrix(std::vector<std::vector<bool>>& init_matrix) : b_matrix(init_matrix) {}

BoolMatrix::BoolMatrix(BoolMatrix& rhs) : b_matrix(rhs.b_matrix) {}

BoolMatrix::BoolMatrix(const std::string& str)   
{
    uint32_t size = uint32_t(sqrt(str.length()));

    if (size*size != str.length()) throw InvalidKey("Invalid line size. The length of the string must be the square of the number!!!");
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
    for (size_t i = 0; i < m_size/2; ++i)
    {
        for (size_t j = 0; j < m_size/2; ++j)
        {
            if (b_matrix[i][j] == true)
            {
                if (b_matrix[j][m_size-i-1] || b_matrix[m_size-i-1][m_size-j-1] || b_matrix[m_size-j-1][i]) throw InvalidKey("Intersections detected in the Cordano lattice!!!"); 
            }
        }
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

std::string BoolMatrix::decryption(const std::string& str)
{
    if (str.length() % (m_size * m_size) != 0)  throw InvalidOpenText("The length of the text is not a multiple of the square of the matrix side!!!");

    size_t blockCount = str.length() / (m_size * m_size);
    std::string resultString;
    std::string temp;

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


std::string BoolMatrix::encryption(const std::string& str)
{
    size_t textSize= m_size*m_size;
    if (str.length() % textSize != 0) throw InvalidOpenText("The length of the text is not a multiple of the square of the matrix side!!!");

    size_t count = str.length()/(textSize);
    std::string temp(textSize, ' ');
    std::string block(temp);
    std::string resultString;
    size_t tempPosition = 0;

    for (size_t c = 0; c < count; ++c)
    {
        block = temp;
        tempPosition = 0;
        for (uint16_t k = 0; k < 4; ++k)
        {
            for (size_t i = 0; i < this->m_size; ++i)
            {
                for (size_t j = 0; j < this->m_size; ++j)
                {
                    if (this->b_matrix[i][j] == 1) 
                    { 
                        block[i * m_size + j] = str[tempPosition];
                        ++tempPosition;
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
    if (i<0 || i >= m_size) throw std::invalid_argument("Invalid index in operator []!!!\n");
    return b_matrix.at(i);
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