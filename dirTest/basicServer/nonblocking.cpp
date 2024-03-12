#include "ServerTool.hpp"

extern int fdMax;

int	main (void) {
	char	port[] = "1600";
	int		client_fd;
	int		server_fd;
	std::string	reqMsg;
	std::string	resMsg;

	server_fd = initServer(port);
	fcntl(server_fd, F_SETFL, O_NONBLOCK); // set server to non-blcoking i/o
	if (server_fd < 0)
		return 1;
	while (true) {
		std::cout << GRN << "Start server" << RESET << std::endl;
		while (true) {
			client_fd = acceptConnection(server_fd);
			if (client_fd != -1)
				break;
		}
		while (true) {
			int status = receiveRequest(client_fd, reqMsg);
			if (status == -1)
				continue;
			else if (status > 0) {
				readFile("text.txt", resMsg);
				sendReponse(client_fd, resMsg);
				continue;
			}
			close(client_fd);
			break;
		}
	}
	return 0;
}