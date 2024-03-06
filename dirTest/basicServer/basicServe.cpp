#include "ServerTool.hpp"

int main(void)
{
	char	port[] = "1600";
	int client_fd;
	int server_fd;
	fd_set readfds;
	std::string reqMsg;

	server_fd = initServer(port);
	if (server_fd < 0)
		return 1;
	while (1)
	{
		client_fd = acceptConnection(server_fd);
		if (client_fd == 0) // can't accept connection
			continue;
		if (receiveRequest(client_fd, reqMsg))
		{
			reqMsg.clear();
			if (readFile("text.txt", reqMsg)) {
				std::cout << MAG << "Test waiting ..." << RESET << std::endl;
				sleep(2);
				sendReponse(client_fd, reqMsg);
			}
		}
		close(client_fd);
		std::cout << GRN << "Close connection" << RESET << std::endl;
	}
	return 0;
}
