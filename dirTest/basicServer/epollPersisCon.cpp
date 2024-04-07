#include "ServerTool.hpp"

extern int fdMax;
extern int fdCount;

#define MAX_EVENTS 20

int	main (void) {
	char	port[] = "1600";
	int		client_fd;
	int		server_fd;
	int		epoll_fd;
	int		nfds; // number of fd that are ready
	std::string	reqMsg;
	struct epoll_event	ep_event[MAX_EVENTS];
	
	epoll_fd = epoll_create(5); // intialize 5 client;
	if (epoll_fd == -1)
		return (perror("epoll create"), 5);
	server_fd = initServer(port);
	if (server_fd < 0)
		return 1;
	if (fdAdd(epoll_fd, server_fd, EPOLLIN) == -1)
		return (perror("epoll_ctl"), 2);
	while (true) {
		std::cout << GRN << "Start select..." << RESET << std::endl;
		nfds = epoll_wait(epoll_fd, ep_event, MAX_EVENTS, -1);
		if (nfds <= 0)
			return (perror("epoll"), 4);
		std::cout << MAG << "nfds: " << nfds << RESET << std::endl;
		for (int i = 0; i < nfds; i++) {
			if (ep_event[i].events & EPOLLIN) {
				if (ep_event[i].data.fd == server_fd) { // handle new connection
					client_fd = acceptConnection(server_fd);
					if (client_fd == 0) // can't accept connection
						continue;
					if (fdAdd(epoll_fd, client_fd, EPOLLIN) == -1)
						perror("epoll_ctl");
				}
				else { // handle data from client
					receiveRequest(ep_event[i].data.fd, reqMsg);
					fdMod(epoll_fd, ep_event[i].data.fd, EPOLLOUT);
				}
			}
			else if (ep_event[i].events & EPOLLOUT) { // send data back to client
				std::string	resMsg;
				readFile("text.txt", resMsg);
				sendReponse(ep_event[i].data.fd, resMsg);
				// close(ep_event[i].data.fd);
				// fdDel(epoll_fd, ep_event[i].data.fd);
				fdMod(epoll_fd, ep_event[i].data.fd, EPOLLIN);
				// std::cout << GRN << "Close connection" << RESET << std::endl;
			}
		}
	}
	return 0;
}