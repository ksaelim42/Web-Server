#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    // Specify the file path
    std::string filePath = "example.txt";

    // Open the file
    std::ifstream inputFile(filePath);

    // Check if the file is open
    if (inputFile.is_open()) {
        // Create a std::stringstream to store the content
        std::stringstream buffer;

        // Read the file content into the stringstream
        buffer << inputFile.rdbuf();

        // Close the file
        inputFile.close();

        // Get the content as a string
        std::string fileContent = buffer.str();

        // Display the content
        // std::cout << "File Content:\n" << fileContent << std::endl;
        std::cout << "File length:\n" << fileContent.length() << std::endl;
    } else {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }

    return 0;
}
