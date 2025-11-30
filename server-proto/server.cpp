#include <iostream>
#include <sys/socket.h>	// socket(), bind(), listen(), accept()
#include <netinet/in.h>	// sockaddr_in, htons()
#include <poll.h>
#include <vector>
#include <unistd.h>		// close()
#include <fcntl.h>

int	main () {
	int	listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listener_fd < 0) {
		std::cerr << "problem while creating socket" << std::endl;
		return 1;
	}

	int opt = 1;
	setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	fcntl(listener_fd, F_SETFL, O_NONBLOCK);

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(6667);

	if (bind(listener_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		std::cerr << "problem while binding socket and adress" << std::endl;
		return 1;
	}

	if (listen(listener_fd, 10) < 0) {
		std::cerr << "problem while trying to listen to socket" << std::endl;
		return 1;
	}

	std::cout << "server is up, listening to port 6667" << std::endl;

	std::vector<struct pollfd> fds;

	struct pollfd pfd;
	pfd.fd = listener_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	fds.push_back(pfd);

	while (true) {
		if (poll(&fds[0], fds.size(), -1) < 0) {
			std::cerr << "problem while running poll" << std::endl;
			break ;
		}

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == listener_fd) {
					int	new_fd = accept(listener_fd, NULL, NULL);
					if (new_fd < 0) {
						std::cerr << "accept failed" << std::endl;
						continue ;
					}
					fcntl(new_fd, F_SETFL, O_NONBLOCK);

					struct pollfd new_pfd;
					new_pfd.fd = new_fd;
					new_pfd.events = POLLIN;
					new_pfd.revents = 0;
					fds.push_back(new_pfd);

					std::cout << "new client added" << std::endl;
				}
				else {
					char	buffer[1024];
					int	bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0) {
						std::cout << "client: " << fds[i].fd << "removed" << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						i--;
					}
					else {
						buffer[bytes] = '\0';
						std::cout << "Client " << fds[i].fd << ": " << buffer << std::endl;
					}
				}
			}
		}
	}

}