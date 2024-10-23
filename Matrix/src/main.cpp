#include "Matrix.hpp"

int main()
{
    try
    {
        Matrix A(2, 5);
        std::cout << A;
        A[1][1] = 1;
        std::cout << A;
        std::cout << A[1][1];
        A.matrix_random(1, 5);
        
        std::cout << A << "Transpose";
        A.transpose();
        std::cout << A << "Transpose";
        A.transpose();
        std::cout << A;
        

        //Matrix B(A.inverse());
        //std::cout << A << " * " << B << " = " << A;

        Matrix C("(1 2 3)(4 5 6)(7 8 9)(10 11 12)");
        std::cout << C;
        return 0;
    }
    catch (std::exception& a)
    {
        std::cout << a.what();
    }
}
