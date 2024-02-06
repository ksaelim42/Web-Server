#include <iostream>
#include <fstream>
#include <string>

int main() {
    // Specify the file path
    std::string filePath = "example.txt";

    // Open the file
    std::ifstream inputFile(filePath);

    // Check if the file is open
    if (inputFile.is_open()) {
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