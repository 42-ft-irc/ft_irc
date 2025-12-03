#include "channel.hpp"

channel::channel( std::string name, std::string key ) 
: _name(name), _topic(""), _key(key), _limit(0), 
	_inviteOnly(false), _topicRestricted(false) {}
channel::~channel() {}

std::string channel::getName() const { return _name; }
std::string channel::getTopic() const { return _topic; }
std::string channel::getKey() const { return _key; }
int         channel::getLimit() const { return _limit; }
std::string channel::getModes() const {
	std::string modes = "+";
	if (_inviteOnly) modes += "i";
	if (_topicRestricted) modes += "t";
	if (!_key.empty()) modes += "k";
	if (_limit > 0) modes += "l";
	return modes;
}

void channel::setTopic(std::string topic) { _topic = topic; }
void channel::setKey(std::string key) { _key = key; }
void channel::setLimit(int limit) { _limit = limit; }
void channel::setInviteOnly(bool status) { _inviteOnly = status; }
void channel::setTopicRestricted(bool status) { _topicRestricted = status; }


void channel::addClient(client* new_client) { _clients.push_back(new_client); }
void channel::removeClient(client* client_to_remove) {
	std::vector<client*>::iterator it = std::find(_clients.begin(), _clients.end(), client_to_remove);
	if (it != _clients.end()) {
		_clients.erase(it);
	}
	removeOperator(client_to_remove);
}
void channel::addOperator(client* new_op) { _operators.push_back(new_op); }
void channel::removeOperator(client* admin_to_remove) {
	std::vector<client*>::iterator it = std::find(_operators.begin(), _operators.end(), admin_to_remove);
	if (it != _operators.end()) {
		_operators.erase(it);
	}
}

bool channel::isMember(client* c) {
	std::vector<client*>::iterator it = std::find(_clients.begin(), _clients.end(), c);
	return (it != _clients.end());
}
bool channel::isOperator(client* c) {
	std::vector<client*>::iterator it = std::find(_operators.begin(), _operators.end(), c);
	return (it != _operators.end());
}

void channel::broadcast(std::string message, client* sender) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] != sender) {
			int fd = _clients[i]->getFD();
			send(fd, message.c_str(), message.size(), 0);
		}
	}
}