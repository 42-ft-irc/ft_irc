#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "libraries.hpp"

class client;

class channel
{
	private:
		// Disabled
		channel();
		channel(const channel& src);
		channel& operator=(const channel& src);

		// Channel info
		std::string	_name;
		std::string	_topic;
		std::string	_key;

		// Mode flags
		bool	_inviteOnly;
		bool	_topicRestricted;
		int		_limit;

		// Member lists
		std::vector<client*>	_operators;
		std::vector<client*>	_clients;
		std::vector<client*>	_invited;
		
	public:
		// Constructors & Destructor
		channel(const std::string& name, const std::string& key);
		~channel();

		// Getters
		const std::string&			getName() const;
		const std::string&			getTopic() const;
		const std::string&			getKey() const;
		int							getLimit() const;
		std::string					getModes() const;
		size_t						getClientCount() const;
		const std::vector<client*>&	getClients() const;

		// State queries
		bool	isMember(const client* c) const;
		bool	isOperator(const client* c) const;
		bool	isInvited(const client* c) const;
		bool	isEmpty() const;
		bool	isInviteOnly() const;
		bool	isTopicRestricted() const;

		// Setters
		void	setTopic(const std::string& topic);
		void	setKey(const std::string& key);
		void	setLimit(int limit);
		void	setInviteOnly(bool status);
		void	setTopicRestricted(bool status);

		// Client management
		void	addClient(client* c);
		void	removeClient(client* c);

		// Operator management
		void	addOperator(client* c);
		void	removeOperator(client* c);

		// Invite management
		void	addInvite(client* c);
		void	removeInvite(client* c);

		// Messaging
		void	broadcast(const std::string& message, client* sender);
};

#endif