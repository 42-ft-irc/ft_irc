#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "libraries.hpp"
#include "client.hpp"

class channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;

		bool	_inviteOnly;
		bool	_topicRestricted;
		int		_limit;

		std::vector<client*> _operators;
		std::vector<client*> _clients;
	
	public:
		channel( std::string name, std::string key );
		~channel();

		std::string	getName() const;
		std::string	getTopic() const;
		std::string	getKey() const;
		int			getLimit() const;
		std::string	getModes() const;

		void	setTopic(std::string topic);
		void	setKey(std::string key);
		void	setLimit(int limit);
		void	setInviteOnly(bool status);
		void	setTopicRestricted(bool status);

		void	addClient(client* new_client);
		void	removeClient(client* client_to_remove);
		void	addOperator(client* new_op);
		void	removeOperator(client* op_to_remove);

		bool	isMember(client* c);
		bool	isOperator(client* c);

		void	broadcast(std::string message, client* sender);
};

#endif