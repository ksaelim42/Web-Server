#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>


struct stat	_fileInfo;


int main() {
    // Specify the file path
    std::string filePath = "../../html/static/images/test girl.png";

    std::cout << "if size: " << sizeof(std::ifstream) << std::endl;
    // Open the file
    std::ifstream inputFile(filePath);
	if (stat(filePath.c_str(), &_fileInfo) == 0)
        std::cout << "File size: " << _fileInfo.st_size << std::endl;

    // Check if the file is open
    if (inputFile.is_open()) {
        std::cout << "fd : " << open("../../html/static/images/test girl.png", O_RDONLY) << std::endl;
        // Create a std::string to store the content
        std::string fileContent;

        // Read the file content into the string
        fileContent.assign(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());

        // Close the file
        inputFile.close();

        // Display the content
        // std::cout << "File Content:\n" << fileContent << std::endl;
        std::cout << "File length:\n" << fileContent.length() << std::endl;
    } else {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }

    return 0;
}