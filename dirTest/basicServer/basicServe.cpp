#include "ServerTool.hpp"

int main(void)
{
	int client_fd;
	int server_fd;
	fd_set readfds;
	std::string reqMsg;
	if (initServer(server_fd) == 0)
		return 1;
	while (1)
	{
		client_fd = acceptConnection(server_fd);
		if (client_fd == 0) // can't accept connection
			continue;
		if (receiveRequest(client_fd, reqMsg))
		{
			std::string	content;
			if (readFile("text.txt", content)) {
				std::cout << MAG << "Test waiting ..." << RESET << std::endl;
				sleep(3);
				sendReponse(client_fd, content);
			}
		}
		close(client_fd);
		std::cout << GRN << "Close connection" << RESET << std::endl;
	}
	return 0;
}
