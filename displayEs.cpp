#include <iostream>
#include <fstream>

bool	readFile(char* fileName, std::string & buffer) {
	std::ifstream	inFile;
	int				length;

	inFile.open(fileName);		// Convert string to char* by c_str() function
	if (!inFile.is_open()) {
		return (false);
	} // : TODO Check it is a file not a dir if dir are there autoindex
	inFile.seekg(0, inFile.end);		// Set position to end of the stream
	length = inFile.tellg();			// Get current position
	inFile.seekg(0, inFile.beg);		// Set position back to begining of the stream
	buffer.resize(length);
	inFile.read(&buffer[0], length);	// Read all data in inFile to Buffer
	inFile.close();						// Close inFile
	return (true);
}

int	main(int ac, char **av) {
	std::string	buffer;

	if (readFile(av[1], buffer) == 0)
		std::cerr << "Can't open file" << std::endl;
	else {
		for (int i = 0; i < buffer.size(); i++)
			if (buffer[i] == '\r')
				std::cout << "\\r";
			else if (buffer[i] == '\n')
				std::cout << "\\n" << std::endl;
			else
				std::cout << buffer[i];
	}
}
