#include <cstdio>
#include <unordered_map>
#include <iostream>
int main()
{
	std::unordered_map<int, MyClass> test;
	test[1] = MyClass();
	test[2] = MyClass();
	test[100] = MyClass(1000, 10.0);

	for (auto x : test) {
		std::cout << x.first;
		std::cout << x.second << std::endl;
	}
    return 0;
}
