#include "ServerTool.hpp"

int main(void)
{
	char	port[] = "1600";
	int client_fd;
	int server_fd;
	std::string reqMsg;

	server_fd = initServer(port);
	if (server_fd < 0)
		return 1;
	while (1)
	{
		client_fd = acceptConnection(server_fd);
		if (client_fd == 0) // can't accept connection
			continue;
		while (receiveRequest(client_fd, reqMsg))
			continue;
		close(client_fd);
		std::cout << GRN << "Close connection" << RESET << std::endl;
	}
	return 0;
}
