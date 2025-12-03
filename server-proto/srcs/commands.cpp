#include "server.hpp"

void server::initCommands( void ) {
	_commands["PASS"] = &server::handlePass;
	_commands["CAP"] = &server::handleCap;
	_commands["NICK"] = &server::handleNick;
	_commands["USER"] = &server::handleUser;
	_commands["PING"] = &server::handlePing;
	_commands["QUIT"] = &server::handleQuit;
}

static void sendReply(int fd, const std::string& msg) {
	std::string full = msg + "\r\n";
	send(fd, full.c_str(), full.size(), 0);
}

void server::welcomeClient(int fd) {
	client* c = _clients[fd];
	std::string nick = c->getNickname();

	c->setRegistered(true);

	std::ostringstream userCount;
	userCount << _clients.size();

	sendReply(fd, ":server " RPL_WELCOME " " + nick + " :Welcome to the IRC Network " + nick);
	sendReply(fd, ":server " RPL_YOURHOST " " + nick + " :Your host is server, running version 1.0");
	sendReply(fd, ":server " RPL_CREATED " " + nick + " :This server was created today");
	sendReply(fd, ":server " RPL_MYINFO " " + nick + " server 1.0 o o");
	sendReply(fd, ":server " RPL_LUSERME " " + nick + " :I have " + userCount.str() + " clients and 1 servers");
	sendReply(fd, ":server " RPL_MOTDSTART " " + nick + " :- server Message of the Day -");
	sendReply(fd, ":server " RPL_MOTD " " + nick + " :- Welcome to ft_irc!");
	//sendReply(fd, ":server " RPL_ENDOFMOTD " " + nick + " :End of /MOTD command.");
}

void server::handlePass(int fd, message &msg) {
	client* c = _clients[fd];

	if (c->isRegistered()) {
		sendReply(fd, ":server 462 " + c->getNickname() + " :You may not reregister");
		return;
	}
	if (msg.params.empty()) {
		sendReply(fd, ":server 461 * PASS :Not enough parameters");
		return;
	}
	if (msg.params[0] != _password) {
		sendReply(fd, ":server " ERR_PASSWDMISMATCH " * :Password incorrect");
		return;
	}
	c->setAuthenticated(true);
}

void server::handleCap(int fd, message &msg) {
	if (msg.params.size() > 0 && msg.params[0] == "LS") {
		sendReply(fd, "CAP * LS :");
	}
}

void server::handleNick(int fd, message &msg) {
	client* c = _clients[fd];

	if (!c->isAuthenticated()) {
		sendReply(fd, ":server " ERR_NOTREGISTERED " * :You have not registered");
		return;
	}
	if (msg.params.empty()) {
		sendReply(fd, ":server 431 * :No nickname given");
		return;
	}

	std::string oldNick = c->getNickname();
	std::string newNick = msg.params[0];
	c->setNickname(newNick);

	if (c->isRegistered()) {
		sendReply(fd, ":" + oldNick + " NICK " + newNick);
	} else if (!c->getNickname().empty() && !c->getUsername().empty()) {
		welcomeClient(fd);
	}
}

void server::handleUser(int fd, message &msg) {
	client* c = _clients[fd];

	if (!c->isAuthenticated()) {
		sendReply(fd, ":server " ERR_NOTREGISTERED " * :You have not registered");
		return;
	}
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

void server::handleQuit(int fd, message &msg) {
	client* c = _clients[fd];
	std::string quitMsg = "Client Quit";

	if (!msg.params.empty())
		quitMsg = msg.params[0];

	std::string nick = c->getNickname().empty() ? "*" : c->getNickname();
	sendReply(fd, "ERROR :Closing Link: " + nick + " (Quit: " + quitMsg + ")");
}
