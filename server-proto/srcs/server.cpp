#include "server.hpp"

extern bool g_server_running;

server::server( int port, const std::string& password ) 
	: _port(port), _password(password), _listenerFd(-1) {
	createSocket();
	bindAndListen();
	initCommands();
}
server::~server() {
	std::cout << "\nShutting down server..." << std::endl;
	for (std::map<int, client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
		close(it->first);
	}
	_clients.clear();
	for (std::map<std::string, channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
	if (_listenerFd != -1)
		close(_listenerFd);
}

void server::createSocket() {
	_listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenerFd < 0)
		throw ServerException("Failed to create socket");

	int opt = 1;
	if (setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw ServerException("Failed to set SO_REUSEADDR");

	if (fcntl(_listenerFd, F_SETFL, O_NONBLOCK) < 0)
		throw ServerException("Failed to set O_NONBLOCK");

	struct pollfd pfd;
	pfd.fd = _listenerFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
}

void server::bindAndListen() {
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_port);

	if (bind(_listenerFd, (struct sockaddr*)&address, sizeof(address)) < 0)
		throw ServerException("sock de la flop");

	if (listen(_listenerFd, SOMAXCONN) < 0)
		throw ServerException("Failed to listen on socket");
	
	std::cout << "Server listening on port " << _port << std::endl;
}

void server::run() {
	while (g_server_running) {
		int ret = poll(_pollFds.data(), _pollFds.size(), TIMEOUT_POLL); 
		if (ret <= 0) {
			if (errno == EINTR || ret == 0) continue;
			throw ServerException("Poll error");
		}

		for (size_t i = 0; i < _pollFds.size(); ++i) {
			if (_pollFds[i].revents & POLLIN) {
				if (_pollFds[i].fd == _listenerFd) {
					acceptNewClient();
				} else {
					handleClientData(_pollFds[i].fd);
				}
			}
			if (_pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
				if (_pollFds[i].fd != _listenerFd)
					removeClient(_pollFds[i].fd);
			}
		}
	}
}

void server::acceptNewClient() {
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newFd = accept(_listenerFd, (struct sockaddr*)&clientAddr, &clientLen);
	
	if (newFd < 0) {
		if (errno != EWOULDBLOCK) std::cerr << "Accept failed" << std::endl;
		return;
	}

	fcntl(newFd, F_SETFL, O_NONBLOCK);

	_clients[newFd] = new client(newFd);

	struct pollfd pfd;
	pfd.fd = newFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);

	std::cout << "New client connected: " << newFd << std::endl;
}

void server::handleClientData(int fd) {
	char buffer[BUFFER_SIZE];
	std::memset(buffer, 0, sizeof(buffer));

	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0) {
		removeClient(fd);
	} else {
		if (_clients.find(fd) != _clients.end()) {
			_clients[fd]->appendToBuffer(buffer);
			client* c = _clients[fd];
			size_t pos;
			while ((pos = c->getRecvBuffer().find("\r\n")) != std::string::npos) {
				std::string msgStr = c->getRecvBuffer().substr(0, pos);
				c->eraseFromBuffer(pos + 2);
				
				if (msgStr.empty()) continue;

				std::cout << "[Client " << fd << "]: " << msgStr << std::endl;

				message msg = parseMessage(msgStr);
				executeCommand(fd, msg);
			}
		}
	}
}

void server::removeClient(int fd) {
	if (_clients.find(fd) == _clients.end()) return;

	std::cout << "Client disconnected: " << fd << std::endl;

	for (std::map<std::string, channel*>::iterator it = _channels.begin(); it != _channels.end(); ) {
		channel* chan = it->second;
		if (chan->isMember(_clients[fd])) {
			chan->removeClient(_clients[fd]);

			if (chan->getClientCount() == 0) {
				delete chan;
				std::map<std::string, channel*>::iterator toErase = it;
				++it;
				_channels.erase(toErase);
				std::cout << "Channel deleted (empty) after client disconnect." << std::endl;
				continue;
			}
		}
		++it;
	}

	removeFromPoll(fd);

	close(fd);
	delete _clients[fd];
	_clients.erase(fd);
}

void server::removeFromPoll(int fd) {
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == fd) {
			_pollFds.erase(it);
			break;
		}
	}
}

channel* server::getChannel(const std::string& name) {
	if (_channels.find(name) != _channels.end())
		return _channels[name];
	return NULL;
}

channel* server::createChannel(const std::string& name, const std::string& key, client* creator) {
	channel* newChan = new channel(name, key);
	if (creator) {
		newChan->addClient(creator);
		newChan->addOperator(creator);
	}
	_channels[name] = newChan;
	return newChan;
}

void server::removeChannel(const std::string& name) {
	if (_channels.find(name) != _channels.end()) {
		delete _channels[name];
		_channels.erase(name);
	}
}
