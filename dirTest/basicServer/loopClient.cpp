#include "ServerTool.hpp"

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
		std::cout << GRN << "success fd: " << client_fd << RESET << std::endl;

	// Set address and port of server that need to connect to
	server_addr.sin_family = AF_INET; // set for IPv4
	server_addr.sin_port = htons(8003); // convert host byte order to network byte order

	// inet_addr is function for convert string form standard IPv4 dotted decimal notation
	// to interger value that suitable for internet address.
	server_addr.sin_addr.s_addr = inet_addr(ADDR); // IP address of server

	// Connect
	status = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	std::cout << "connect to server..." << "port 8003" << std::endl;
	if (status < 0) {
		std::cout << RED << "failed" << RESET << std::endl;
		exit(1);
	}
	std::cout << GRN << "success, connected to server Type data for send" << RESET << std::endl;
	std::string	str;
	// Sent data to server
	char	resMsg[1024];
	while (true) {
		std::cout << "> ";
		getline(std::cin, str);
		memset(resMsg, 0, 1024);
		send(client_fd, str.c_str(), str.length(), 0);
		// read data from server
		read(client_fd, resMsg, 1024);
		std::cout << YEL << resMsg << RESET << std::endl;
	}
	close(client_fd);
	return 0;
}
