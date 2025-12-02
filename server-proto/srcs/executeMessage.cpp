#include "server.hpp"

void	server::executeMessage( int sender_fd, message msg) {
	for (size_t j = 0; j < _fds.size(); j++) {
		int dest_fd = _fds[j].fd;
		if (dest_fd != _listener_fd && dest_fd != sender_fd) {
			if (send(dest_fd, msg.params.back().c_str(), msg.params.back().size(), 0) < 0) {
				std::cerr << "problem while sending to: " << dest_fd << std::endl;
			}
		}
	}
}