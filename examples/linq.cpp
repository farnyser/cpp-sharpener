#include <cstdlib>
#include <iostream>
#include <vector>
#include "../linq/src/IEnumerable.hpp"

using namespace linq;

int main(int argc, char **argv)
{
	auto data = IEnumerable<int>::Range(0, 10000);
	
	std::cout << "data count: " << data.Count() << std::endl;
	std::cout << "     first: " << data.First() << std::endl;
	std::cout << "      last: " << data.Last() << std::endl;
	
	auto data_filterd = data.Where(x => x % 5 == 0);
	
	std::cout << "divisible by 5, and lower than 30: " << std::endl;
	for(auto& x : data_filterd.TakeWhile(x => x < 30))
		std::cout << " # " << x << std::endl;
	
	return EXIT_SUCCESS;
}
