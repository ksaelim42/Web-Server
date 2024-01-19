#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include  <string.h>

# define RED	"\e[0;31m"
# define GREEN	"\e[0;32m"
# define RESET	"\e[0m"

int	main(void)
{
	int					status;
	int					client_fd;
	unsigned short int	netPort;
	struct sockaddr_in	server_addr;

	// Creating socket file descriptor
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "create socket: ";
	if (client_fd < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "success fd: " << client_fd << RESET << std::endl;

	// Set address and port of server that need to connect to
	server_addr.sin_family = AF_INET; // set for IPv4
	server_addr.sin_port = htons(8080); // convert host byte order to network byte order

	// inet_addr is function for convert string form standard IPv4 dotted decimal notation
	// to interger value that suitable for internet address.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address of server

	// Connect
	status = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	std::cout << "connect to server..." << std::endl;
	if (status < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}

	std::cout << GREEN << "success, connected to server" << RESET << std::endl;

	std::string	str = "Hello, I am from client...";

	// Sent data to server
	send(client_fd, str.c_str(), str.length(), 0);
}