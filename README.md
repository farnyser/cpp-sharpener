C++ sharpener
==============

Mini project aiming to add some C# flavor to C++ !

Example
-------

Build a lambda C# style:

	auto square = x => x * x;
	std::cout << square(2) << std::endl; // output 4
	
Bild & Usage
------------

	g++ -std=c++14 main.cpp -o sharpener
	cat example.cpp | ./sharpener | g++ -xc++ -std=c++14 -o example -
	./example