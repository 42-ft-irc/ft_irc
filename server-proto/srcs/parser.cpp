#include "server.hpp"

message	server::parseMessage( const std::string &raw_data ) const {
	message				msg;
	std::stringstream	ss(raw_data);
	std::string			param;

	ss >> param;
	if ( param[0] == ':' ) {
		msg.prefix = param.substr(1);
		ss >> msg.command;
	}
	else {
		msg.command = param;
	}
	while (ss >> param) {
		if (param.empty())
			break;
		if ( param[0] == ':' ) {
			std::string	rest;
			std::getline(ss, rest);
			param = param.substr(1) + rest;
			if (!param.empty() && param[param.size() - 1] == '\r') {
				param.erase(param.size() - 1);
			}
		}
		msg.params.push_back(param);
	}
	std::cout << msg << std::endl;
	return (msg);
}