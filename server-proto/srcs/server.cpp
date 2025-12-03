#include "server.hpp"

server::server( void ) : _port(PORT) {}
server::server( int port ) : _port(port){}
server::~server() {
	for (std::map<int, client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
		close(it->first);
	}
	_clients.clear();
	_fds.clear();
}

int	server::startServer( void ) {
	_listener_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listener_fd < 0)
		throw ServerException("Failed to create socket");

	int opt = 1;
	if (setsockopt(_listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw ServerException("Failed to set SO_REUSEADDR");

	if (fcntl(_listener_fd, F_SETFL, O_NONBLOCK) < 0)
		throw ServerException("Failed to set O_NONBLOCK");

	setAddress();

	if (bind(_listener_fd, reinterpret_cast<struct sockaddr*>(&_address), sizeof(_address)) < 0)
		throw ServerException("Failed to bind socket");

	if (listen(_listener_fd, 10) < 0)
		throw ServerException("Failed to listen on socket");

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
					addFD(new_fd);
					_clients[new_fd] = new client(new_fd);
				}
				else {
					char	buffer[BUFFER_SIZE];
					int	bytes = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes <= 0)
						i = removeClient( _fds[i].fd, i );
					else {
						buffer[bytes] = '\0';
						_clients[_fds[i].fd]->appendToBuffer(buffer);
						processClientCommands(_fds[i].fd);
					}
				}
			}
		}
	}
	return (0);
}

int	server::removeClient( int fd, int i ) {
	std::cout << "client: " << fd << " removed" << std::endl;
	if (_clients.find(fd) != _clients.end()) {
		delete _clients[fd];
		_clients.erase(fd);
	}
	close(fd);
	_fds.erase(_fds.begin() + i);
	i--;
	return (i);
}

void	server::setAddress( void ) {
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
	std::cout << "new client added" << std::endl;
}

void	server::processClientCommands( int fd ) {
	client* c = _clients[fd];
	size_t pos;

	while ((pos = c->getRecvBuffer().find("\r\n")) != std::string::npos) {
		std::string single_msg = c->getRecvBuffer().substr(0, pos);
		c->eraseFromBuffer(pos + 2);

		if (single_msg.empty())
			continue;

		try {
			message msg = parseMessage(single_msg);
			executeMessage(fd, msg);
		}
		catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}
}
