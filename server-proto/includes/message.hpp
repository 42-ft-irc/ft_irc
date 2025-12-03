#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>
#include <iostream>

struct message {
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
};

inline std::ostream	&operator << (std::ostream& out, const message& msg) {
	out << "Prefix: " << (msg.prefix.empty() ? "(none)" : msg.prefix) << " , ";
    out << "Cmd: " << msg.command << " , ";
    out << "Params: ";
    for (size_t i = 0; i < msg.params.size(); i++) {
        out << "'" << msg.params[i] << "' ";
    }
    return (out);
}

#endif