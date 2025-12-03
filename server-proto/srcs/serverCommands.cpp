#include "server.hpp"

void server::handleCap(int fd, message &msg) {
	if (msg.params.size() > 0 && msg.params[0] == "LS") {
		sendReply(fd, "CAP * LS :");
	}
}

void server::handleNick(int fd, message &msg) {
	client* c = _clients[fd];

	if (msg.params.empty()) {
		sendReply(fd, ":server 431 * :No nickname given");
		return;
	}
	c->setNickname(msg.params[0]);

	if (!c->isRegistered() && !c->getNickname().empty() && !c->getUsername().empty()) {
		welcomeClient(fd);
	}
}

void server::handleUser(int fd, message &msg) {
	client* c = _clients[fd];

	if (msg.params.size() < 4) {
		sendReply(fd, ":server 461 * USER :Not enough parameters");
		return;
	}
	if (c->isRegistered()) {
		sendReply(fd, ":server 462 " + c->getNickname() + " :You may not reregister");
		return;
	}
	c->setUsername(msg.params[0]);

	if (!c->isRegistered() && !c->getNickname().empty() && !c->getUsername().empty()) {
		welcomeClient(fd);
	}
}

void server::handlePing(int fd, message &msg) {
	std::string response = "PONG :";
	if (!msg.params.empty())
		response += msg.params[0];
	sendReply(fd, response);
}
