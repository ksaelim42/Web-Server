#include <iostream>
#include <vector>

int	main(void)
{
	// std::vector<char *>	str;
	// str = "Hello World";
	std::string	str = "Hello";
	std::cout << str.length() << std::endl;
	std::cout << (int)str[5] << std::endl;
	// std::cout << str << std::endl;
}