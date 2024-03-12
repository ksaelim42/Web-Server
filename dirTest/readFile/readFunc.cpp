#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cerrno>

bool	readFile(std::string & fileName, std::string & buffer) {
	std::ifstream	inFile;
	int				length;

	std::cout << "errno: " << errno << std::endl;
	inFile.open(fileName.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open()) {
		if (errno == ENOENT) {	// 2 No such file or directory : 404
			std::cerr << "No such file or directory" << std::endl;
			return false;
		}
		if (errno == EACCES) { // 13 Permission denied : 403
			std::cerr << "Permission denied" << std::endl;
			return false;
		}
		// EMFILE, ENFILE : Too many open file, File table overflow
		// Server Error, May reach the limit of file descriptors : 500
		std::cerr << "Internal Server Error" << std::endl;
		return (false);
	} // : TODO Check it is a file not a dir if dir are there autoindex
	inFile.seekg(0, inFile.end);		// Set position to end of the stream
	length = inFile.tellg();			// Get current position
	inFile.seekg(0, inFile.beg);		// Set position back to begining of the stream
	buffer.resize(length);
	inFile.read(&buffer[0], length);	// Read all data in inFile to Buffer
	inFile.close();						// Close inFile
	std::cout << "File length:\n" << buffer.length() << std::endl;
	return (true);
}

int main() {
	// Specify the file path
	// std::string filePath = "notExisting";
	// std::string filePath = "example.txt";
	std::string filePath = "dir";
	std::string	buffer;

	readFile(filePath, buffer);
	// Open the file
	// std::ifstream inputFile(filePath);

	// Check if the file is open
	// if (inputFile.is_open()) {
	//     // Create a std::string to store the content
	//     std::string fileContent;

	//     // Get the size of the file
	//     inputFile.seekg(0, std::ios::end);
	//     size_t fileSize = inputFile.tellg();
	//     inputFile.seekg(0, std::ios::beg);
	//     // inputFile.seekg(0, inputFile.end);
	//     // size_t fileSize = inputFile.tellg();
	//     // inputFile.seekg(0, inputFile.beg);

	//     // Resize the string to accommodate the entire file
	//     fileContent.resize(fileSize);

	//     // Read the file content into the string
	//     inputFile.read(&fileContent[0], fileSize);

	//     // Close the file
	//     inputFile.close();

	//     // Display the content
	//     // std::cout << "File Content:\n" << fileContent << std::endl;
	//     std::cout << "File length:\n" << fileContent.length() << std::endl;
	// } else {
	//     std::cerr << "Error opening file: " << filePath << std::endl;
	// }

	return 0;
}
