#include "ServerTool.hpp"

extern int fdMax;
extern int fdCount;

int	main (void) {
	char	port[] = "1600";
	int		client_fd;
	int		server_fd;
	std::string	reqMsg;
	std::vector<struct pollfd>	pfds;

	pfds.reserve(100); // reserve for 100 client for not reallocate
	server_fd = initServer(port);
	if (server_fd < 0)
		return 1;
	fdSet(pfds, server_fd, POLLIN);
	while (true) {
		std::cout << GRN << "Start select..." << RESET << std::endl;
		if (poll(&pfds[0], fdCount, -1) == -1)
			return (perror("poll"), 4);
		for (int i = 0; i < fdCount; i++) {
			if (pfds[i].revents & POLLIN) {
				if (pfds[i].fd == server_fd) { // handle new connection
					client_fd = acceptConnection(server_fd);
					if (client_fd == 0) // can't accept connection
						continue;
					fdSet(pfds, client_fd, POLLIN);
				}
				else { // handle data from client
					receiveRequest(pfds[i].fd, reqMsg);
					pfds[i].events = POLLOUT;
				}
			}
			else if (pfds[i].revents & POLLOUT) { // send data back to client
				std::string	resMsg;
				readFile("text.txt", resMsg);
				sendReponse(pfds[i].fd, resMsg);
				close(pfds[i].fd);
				fdClear(pfds, i);
				std::cout << GRN << "Close connection" << RESET << std::endl;
			}
		}
		sleep(1);
	}
	return 0;
}