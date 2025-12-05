#include "channel.hpp"
#include "client.hpp"

// Constructors & Destructor

channel::channel(const std::string& name, const std::string& key) :
	_name(name), _topic(""), _key(key),
	_inviteOnly(false), _topicRestricted(false), _limit(0) {}

channel::~channel() {}

// Getters

const std::string&			channel::getName() const { return _name; }
const std::string&			channel::getTopic() const { return _topic; }
const std::string&			channel::getKey() const { return _key; }
int							channel::getLimit() const { return _limit; }
size_t						channel::getClientCount() const { return _clients.size(); }
const std::vector<client*>&	channel::getClients() const { return _clients; }

std::string	channel::getModes() const {
	std::string modes = "+";
	if (_inviteOnly) modes += "i";
	if (_topicRestricted) modes += "t";
	if (!_key.empty()) modes += "k";
	if (_limit > 0) modes += "l";
	return modes;
}

// Setters

void channel::setTopic(const std::string& topic) { _topic = topic; }
void channel::setKey(const std::string& key) { _key = key; }
void channel::setLimit(int limit) { _limit = limit; }
void channel::setInviteOnly(bool status) { _inviteOnly = status; }
void channel::setTopicRestricted(bool status) { _topicRestricted = status; }

// State Queries

bool channel::isMember(const client* c) const {
	return (std::find(_clients.begin(), _clients.end(), c) != _clients.end());
}

bool channel::isOperator(const client* c) const {
	return (std::find(_operators.begin(), _operators.end(), c) != _operators.end());
}

bool channel::isEmpty() const { return _clients.empty(); }
bool channel::isInviteOnly() const { return _inviteOnly; }
bool channel::isTopicRestricted() const { return _topicRestricted; }

bool channel::isInvited(const client* c) const {
	std::vector<client*>::const_iterator it = std::find(_invited.begin(), _invited.end(), c);
	return (it != _invited.end());
}

// Client Management

void channel::addClient(client* new_client) { _clients.push_back(new_client); }

void channel::removeClient(client* client_to_remove) {
	std::vector<client*>::iterator it = std::find(_clients.begin(), _clients.end(), client_to_remove);
	if (it != _clients.end()) {
		_clients.erase(it);
	}
	removeOperator(client_to_remove);
}

// Operator Management

void channel::addOperator(client* new_op) { _operators.push_back(new_op); }

void channel::removeOperator(client* admin_to_remove) {
	std::vector<client*>::iterator it = std::find(_operators.begin(), _operators.end(), admin_to_remove);
	if (it != _operators.end()) {
		_operators.erase(it);
	}
}

// Invite Management

void channel::addInvite(client* c) { _invited.push_back(c); }

void channel::removeInvite(client* c) {
	std::vector<client*>::iterator it = std::find(_invited.begin(), _invited.end(), c);
	if (it != _invited.end()) {
		_invited.erase(it);
	}
}

// Messaging

void channel::broadcast(const std::string& message, client* sender) {
	std::string fullMsg = message + "\r\n";

	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] != sender) {
			int fd = _clients[i]->getFD();
			send(fd, fullMsg.c_str(), fullMsg.size(), 0);
		}
	}
}