#include <iostream>
#include <vector>

std::string	getContentType(std::string path) {
	size_t	index = path.find_last_of(".");
	// std::string	extension = path.find_last_of(".");
	if (index == std::string::npos)
		return "Content-Type: text/plain\r\n";
		std::cout << index << "Error" << std::string::npos << std::endl;
	else
		std::cout << path.at(index) << std::endl;
	return "Hello";
}

int	main(void)
{
	getContentType("cat");
	// std::string	str = "
	// Hello";
	// std::vector<char *>	str;
	// str = "Hello World";
	// std::string	str = "Hello";
	// std::cout << str.length() << std::endl;
	// std::cout << (int)str[5] << std::endl;
	// std::cout << str << std::endl;
	// std::cout << str << std::endl;
}