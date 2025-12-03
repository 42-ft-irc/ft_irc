#include "client.hpp"

client::client( int fd ) : _fd(fd), _isRegistered(false) {}

client::~client() {}

int	client::getFD() const {
	return _fd;
}

const std::string&	client::getRecvBuffer( void ) const {
	return _recv_buffer;
}

void	client::appendToBuffer( const std::string &data ) {
	_recv_buffer += data;
}

void	client::eraseFromBuffer( size_t len ) {
	_recv_buffer.erase(0, len);
}