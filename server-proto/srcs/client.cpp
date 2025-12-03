#include "client.hpp"

client::client( int fd ) : _fd(fd), _isRegistered(false) {}
client::~client() {}

int	client::getFD() const {
	return (_fd);
}