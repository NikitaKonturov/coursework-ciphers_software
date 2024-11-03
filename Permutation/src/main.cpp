#include "Permutation.hpp"

int main()
{
	try
	{
		Permutation A(5);
		std::cin >> A;
		std::string str("HelloWorld");
		A.apply(str);
		std::cout << str;
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}