#include "server.hpp"

server::server( void ) : _port(6667) {}
server::server( int port ) : _port(port){}
server::~server() {}

int	server::startServer( void ) {
	_listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listener_fd < 0) {
		std::cerr << "problem while creating socket" << std::endl;
		return 1;
	}

	int opt = 1;
	setsockopt(_listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	fcntl(_listener_fd, F_SETFL, O_NONBLOCK);

	setAdress();

	if (bind(_listener_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "problem while binding socket and adress" << std::endl;
		return 1;
	}

	if (listen(_listener_fd, 10) < 0) {
		std::cerr << "problem while trying to listen to socket" << std::endl;
		return 1;
	}

	std::cout << "server is up, listening to port " << _port << std::endl;

	return (0);
}

int	server::runServerLoop( void ) {
	addFD( _listener_fd );

	while (true) {
		if (poll(&_fds[0], _fds.size(), -1) < 0) {
			std::cerr << "problem while running poll" << std::endl;
			break ;
		}

		for (size_t i = 0; i < _fds.size(); i++) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _listener_fd) {
					int	new_fd = accept(_listener_fd, NULL, NULL);
					if (new_fd < 0) {
						std::cerr << "accept failed" << std::endl;
						continue ;
					}
					fcntl(new_fd, F_SETFL, O_NONBLOCK);

					struct pollfd new_pfd;
					new_pfd.fd = new_fd;
					new_pfd.events = POLLIN;
					new_pfd.revents = 0;
					_fds.push_back(new_pfd);

					std::cout << "new client added" << std::endl;
				}
				else {
					char	buffer[1024];
					int	bytes = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0) {
						std::cout << "client: " << _fds[i].fd << "removed" << std::endl;
						close(_fds[i].fd);
						_fds.erase(_fds.begin() + i);
						i--;
					}
					else {
						buffer[bytes] = '\0';
						std::cout << "Client " << _fds[i].fd << ": " << buffer << std::endl;

						int sender_fd = _fds[i].fd;
						for (size_t j = 0; j < _fds.size(); j++) {
							int dest_fd = _fds[j].fd;
							if (dest_fd != _listener_fd && dest_fd != sender_fd) {
								if (send(dest_fd, buffer, bytes, 0) < 0) {
									std::cerr << "problem while sending to: " << dest_fd << std::endl;
								}
							}
						}
					}
				}
			}
		}
	}
	return (0);
}

void	server::setAdress( void ) {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

void	server::addFD( int new_fd ) {
	struct pollfd pfd;
	pfd.fd = new_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);
}
