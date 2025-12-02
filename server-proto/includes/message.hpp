#pragma once
#include <string>
#include <vector>

struct message {
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
};