#include "client.hpp"

client::client( int fd ) : _fd(fd), _nickname(""), _username(""), _isAuthenticated(false), _isRegistered(false) {}
client::~client() {}

int		client::getFD() const {	return _fd; }
const	std::string&	client::getNickname() const { return _nickname; }
const	std::string&	client::getUsername() const { return _username; }
const	std::string&	client::getRecvBuffer( void ) const { return _recvBuffer; }
bool	client::isAuthenticated() const { return _isAuthenticated; }
bool	client::isRegistered() const { return _isRegistered; }

void	client::setNickname(const std::string& nick) { _nickname = nick; }
void	client::setUsername(const std::string& user) { _username = user; }
void	client::setAuthenticated(bool status) { _isAuthenticated = status; }
void	client::setRegistered(bool status) { _isRegistered = status; }

void	client::appendToBuffer( const std::string &data ) { _recvBuffer += data; }
void	client::eraseFromBuffer(size_t len) {
	if (len > _recvBuffer.size())
		_recvBuffer.clear();
	else
		_recvBuffer.erase(0, len);
}
void	client::clearBuffer() {	_recvBuffer.clear(); }