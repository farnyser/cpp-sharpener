C++ sharpener
==============

Mini project aiming to add some C# flavor to C++ !

Example
-------

Build a lambda C# style:

	auto square = x => x * x;
	std::cout << square(2) << std::endl; // output "4"
	
Simple named function:

	int foobar() => 42;

Null-coalescing operator:

	int a = 0;
	std::cout << a ?? 3 << std::endl; // output "3" 

Null-conditional operator:

	struct foo { auto bar() => "hello !"; };
	std::shared_ptr<foo> a = nullptr;
	std::cout << (a?->bar() ?? "null") << std::endl; // output "null"
	
Build & Usage
------------

	g++ -std=c++14 main.cpp -o sharpener
	cat example.cpp | ./sharpener | g++ -xc++ -std=c++14 -o example -
	./example
