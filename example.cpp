#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

template <typename DATA, typename T>
void foreach(const DATA& data, const T& f)
{
	for(auto& d : data) 
		f(d);
}

auto identity(auto x = {42}) => x; 

struct foo { auto bar() => "hello"; };

int main(int argc, char **argv)
{
	int a = 0;
	std::shared_ptr<foo> b = nullptr;
	
	std::cout << (b?->bar() ?? "null") << std::endl;
	std::cout << identity(a ?? 5) << std::endl;
	std::cout << (x => x * x)(3) << std::endl;
	std::cout << (x => x * x)(0 ?? 7) << std::endl;
	std::cout << (x => x * x)(5) << std::endl;
	std::cout << (x => x * 2)(3) << std::endl;
	std::cout << (() => "hello")() << std::endl;
	std::cout << ((x, y) => x * y)(3, 9) << std::endl;
	
	foreach(std::vector<int>{1, 2, 3}, x => { std::cout << "-> " << x << std::endl; });
	
	return EXIT_SUCCESS;
}

/*
	data.where(x => x.any(y => y > 10))
*/
