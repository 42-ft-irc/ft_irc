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

const std::string&	client::getNickname() const { return _nickname; }
const std::string&	client::getUsername() const { return _username; }
bool	client::isRegistered() const { return _isRegistered; }

void	client::setNickname(const std::string& nick) { _nickname = nick; }
void	client::setUsername(const std::string& user) { _username = user; }
void	client::setRegistered(bool status) { _isRegistered = status; }