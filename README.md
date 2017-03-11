C++ sharpener
==============

Mini project aiming to add some C# flavor to C++ !

Example
-------

Build a lambda C# style:

```c++
auto square = x => x * x;
std::cout << square(2) << std::endl; // output "4"
```

Simple named function:

```c++
int foobar() => 42;
```

Null-coalescing operator:

```c++
int a = 0;
std::cout << a ?? 3 << std::endl; // output "3" 
```

Null-conditional operator:

```c++
struct foo { auto bar() => "hello !"; };
std::shared_ptr<foo> a = nullptr;
std::cout << (a?->bar() ?? "null") << std::endl; // output "null"
```
	
Build & Usage
------------

```shell
g++ -std=c++14 sharpener.cpp -o sharpener
cat examples/example.cpp | ./sharpener | g++ -xc++ -std=c++14 -o example -
./example
```