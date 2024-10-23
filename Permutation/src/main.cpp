#include "Permutation.hpp"

int main()
{
	try
	{
		std::vector<uint32_t> f({3,1,4,2});
		std::vector<uint32_t> g({3,4,1,2});

		Permutation F(f);
		Permutation G(g);
		std::cout << F << " * \n\n" << G << "=\n";
		F.compose(G);
		std::cout << F;
		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}