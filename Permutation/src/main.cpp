#include "Permutation.hpp"

int main()
{
	try
	{
		std::string a("");
		std::getline(std::cin, a);
		Permutation A(a);
		std::cout << A;	
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}