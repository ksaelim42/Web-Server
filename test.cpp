#include <iostream>
#include <vector>
#include <sstream>

std::string	itoa(size_t num) {
	std::stringstream	ss;
	ss << num;
	return ss.str();
}

int	main(void)
{
	// getContentType("cat.jpg");
	std::string	num = itoa(-234324234324341);
	std::cout << num << std::endl;

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