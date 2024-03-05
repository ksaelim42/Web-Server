#include "ServerTool.hpp"

int		fdMax;

void fdSet(int &fd, fd_set &set)
{
	FD_SET(fd, &set);
	if (fd > fdMax)
		fdMax = fd;
}

void fdClear(int &fd, fd_set &set)
{
	close(fd);
	FD_CLR(fd, &set);
	if (fd == fdMax)
		fdMax--;
}

int	main (void) {
	int		client_fd;
	int		server_fd;
	fd_set	readFds;
	std::string	reqMsg;

	if (initServer(server_fd) == 0)
		return 1;
	FD_ZERO(&readFds);
	fdSet(server_fd, readFds);
	while (true) {
		std::cout << GRN << "Start listen" << RESET << std::endl;
		if (select(fdMax + 1, &readFds, NULL, NULL, NULL) == -1) // select will make system motoring three set, block until some fd ready
			return (perror("select"), 4);
		for (int i = 0; i <= fdMax; i++) {
			if (FD_ISSET(i, &readFds)) { // test if fd still present in sets
				std::cout << "fd is set i: " << i << std::endl;
				if (i == server_fd) { // handle new connection
					client_fd = acceptConnection(server_fd);
					fdSet(client_fd, readFds);
				}
				else { // handle data from client
					receiveRequest(client_fd, reqMsg);
					fdClear(client_fd, readFds);
					std::cout << GRN << "Close connection" << RESET << std::endl;
				}
			}
		}
		sleep(1);
		std::cout << "Waiting for connection..." << std::endl;
	}
	return 0;
}