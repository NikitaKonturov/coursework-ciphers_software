#include "Permutation.hpp"

int main() {
    try {
        Permutation A(5);
        std::cin >> A;
        
        std::string str("HelloWorld");
        A.apply(str);
        std::cout << "After apply: " << str << '\n';
        
        A.inverse();
        A.apply(str);
        std::cout << "After inverse apply: " << str << '\n';  
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}