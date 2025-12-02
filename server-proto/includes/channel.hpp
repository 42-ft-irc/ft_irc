#pragma once
#include "libraries.hpp"
#include "client.hpp"

class channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::vector<client*> _operators;
		std::vector<client*> _clients;
		// Modes: i, t, k, o, l (Invite-only, Topic-rest, Key, Op, Limit)
};