#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

int	main(void) {
	std::cout << "errno: " << errno << std::endl;
	int	fd = open("example.txt", O_RDONLY);
	if (fd > 0) {
		off_t	size = lseek(fd, 0, SEEK_END);

		char *buffer = new char [size];		// Allocate Size of Buffer as size of inFile
		read(fd, buffer, size);
		std::string	str = buffer;
		delete [] buffer;				// Free Buffer
		std::cout << "can read the file" << std::endl;

	} else {
		std::cout << "Can't open file " << "errno: " << errno << std::endl;
	}

}