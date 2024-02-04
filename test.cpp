#include <iostream>
#include <vector>

std::string	getContentType(std::string & path, std::map<std::string, std::string> & mimeType) {
	std::string	contentType = "Content-Type: ";
	size_t	index = path.find_last_of(".");
	if (index != std::string::npos) {
		std::string	ext = path.substr(index + 1);
		if (mimeType.count(ext))
			return contentType + mimeType[ext] + "\r\n";
	}
	return contentType + mimeType["default"] + "\r\n";
}

int	main(void)
{
	getContentType("cat.jpg");
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