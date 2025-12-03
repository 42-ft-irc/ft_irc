#include "server.hpp"

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

	client* existing = findClientByNick(newNick);
	if (existing && existing != c) {
		sendReply(fd, ":server " ERR_NICKNAMEINUSE " " + (oldNick.empty() ? "*" : oldNick) + " " + newNick + " :Nickname is already in use");
		return;
	}

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

void server::handlePrivmsg(int fd, message &msg) {
	client* sender = _clients[fd];
	std::string senderNick = sender->getNickname();

	if (!sender->isRegistered()) {
		sendReply(fd, ":server " ERR_NOTREGISTERED " * :You have not registered");
		return;
	}
	if (msg.params.empty()) {
		sendReply(fd, ":server " ERR_NORECIPIENT " " + senderNick + " :No recipient given (PRIVMSG)");
		return;
	}
	if (msg.params.size() < 2 || msg.params[1].empty()) {
		sendReply(fd, ":server " ERR_NOTEXTTOSEND " " + senderNick + " :No text to send");
		return;
	}

	std::string target = msg.params[0];
	std::string text = msg.params[1];

	if (target[0] == '#') {
		if (_channels.find(target) != _channels.end()) {
			channel* chan = _channels[target];
			// Optional: Prüfen ob Sender Mitglied ist (wenn +n Mode aktiv ist)
			if (chan->isMember(sender)) {
				std::string fullMsg = ":" + senderNick + "!" + sender->getUsername() + "@localhost PRIVMSG " + target + " :" + text;
				chan->broadcast(fullMsg, sender); // Sender ausschließen
			} else {
				sendReply(fd, ":server 404 " + senderNick + " " + target + " :Cannot send to channel");
			}
		} else {
			sendReply(fd, ":server " ERR_NOSUCHNICK " " + senderNick + " " + target + " :No such channel");
		}
		return; 
	}

	client* recipient = findClientByNick(target);
	if (!recipient) {
		sendReply(fd, ":server " ERR_NOSUCHNICK " " + senderNick + " " + target + " :No such nick/channel");
		return;
	}

	std::string fullMsg = ":" + senderNick + "!" + sender->getUsername() + "@localhost PRIVMSG " + target + " :" + text;
	sendReply(recipient->getFD(), fullMsg);
}
