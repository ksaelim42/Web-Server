#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>

# define RED	"\e[0;31m"
# define GREEN	"\e[0;32m"
# define RESET	"\e[0m"
# define PORT	8080

#define HTML_FILE "./content/index.html"
#define IMAGE_FILE "./content/image.png"

bool	readFile(std::string name, std::string &str)
{
	std::ifstream	inFile;
	char*			buffer;
	int				length;

	inFile.open(name.c_str());		// Convert string to char* by c_str() function
	if (!inFile.is_open())
	{
		std::cerr << name << " :Can not open." << std::endl;
		return (false);
	}
	inFile.seekg(0, inFile.end);	// Set position to end of the stream
	length = inFile.tellg();		// Get current position
	inFile.seekg(0, inFile.beg);	// Set position back to begining of the stream
	buffer = new char [length];		// Allocate Size of Buffer as size of inFile
	inFile.read(buffer, length);	// Read all data in inFile to Buffer
	str = buffer;					// Copy string from Buffer to str
	delete [] buffer;				// Free Buffer
	inFile.close();					// Close inFile
	return (true);
}

void	handle_client(int client_fd) {
	char	buffer[1024];

	// Receive data from client
	ssize_t	bytes_received =  recv(client_fd, buffer, 1024 - 1, 0);
	if (bytes_received < 0) {
		std::cerr << "Error receiving data" << std::endl;
		close(client_fd);
		exit(1);
	} else if (bytes_received == 0) {
		std::cerr << "Client disconnected" << std::endl;
		close(client_fd);
		exit(1);
	}
	std::cout << "Data receive: " << std::endl;
	std::cout << "-----------------------------------------" << std::endl;
	std::cout << buffer << std::endl;
	// for (int i = 0; buffer[i]; i++)
	// {
	// 	if (buffer[i + 1] == '\n')
	// 	{
	// 		std::cout << "END";
	// 	}
	// 	std::cout << buffer[i];
	// }
	// std::cout << "END";
	std::cout << "-----------------------------------------" << std::endl;
	return ;
}

void	http_reponse(int client_fd, const char *path) {
	int	status;

	std::string	statusLine = "HTTP/1.1 200 OK\r\n";
	std::string	headers = "Content-Type: text/html\r\n\r\n";
	std::string	body;

	if (readFile(path, body) == 0) {
		close(client_fd);
		exit(1);
	}
	else 
		std::cout << "Read file success" << std::endl;
	std::string	content = statusLine + headers + body;
	status = send(client_fd, content.c_str(), content.length(), 0);
	if (status < 0) {
		std::cerr << "Error to response data" << std::endl;
		close(client_fd);
		exit(1);
	}
	std::cout << "Sent data success" << std::endl;
	return ;
}

int	main(void)
{
	int					status;
	int					server_fd;
	unsigned short int	netPort;
	int					enable;
	struct sockaddr_in	server_address;

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "create socket: ";
	if (server_fd < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "success fd: " << server_fd << RESET << std::endl;

	// To manipulate option for socket and check address is used
	enable = 1;
	status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	std::cout << "setup socket: ";
	if (status < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "success" << RESET << std::endl;

	// Set address and port
	server_address.sin_family = AF_INET; // set for IPv4
	server_address.sin_port = htons(PORT); // convert host byte order to network byte order
	server_address.sin_addr.s_addr = INADDR_ANY; // Bind to all available network interfaces

	// Bind the socket to the specified address and port
	status = bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address));
	std::cout << "bind socket: ";
	if (status < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "success" << RESET << std::endl;

	// Prepare socket for incoming connection
	status = listen(server_fd, 10);
	std::cout << "listen: ";
	if (status < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "success" << RESET << std::endl;

	int					client_fd;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);

	// Accept connection
	std::cout << "Open server with port: " << PORT << std::endl;
	while (1) {
		client_fd	= accept(server_fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (client_fd < 0) {
			std::cout << RED << "accept failed" << RESET << std::endl;
			exit(1);
		}
		else {
			std::cout << GREEN << "connection accepted from " << RESET << std::endl;
			std::cout << "client fd: " << client_fd << std::endl;
			std::cout << "client IP address: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
		}
		handle_client(client_fd);
		// http_reponse(client_fd, HTML_FILE);
		// usleep(500000);
		// http_reponse(client_fd, IMAGE_FILE);
		// close(client_fd);
	}
	sleep(3);
	close(server_fd);
	std::cout << "close server" << std::endl;
}