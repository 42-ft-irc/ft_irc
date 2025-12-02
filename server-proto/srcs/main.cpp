#include "server.hpp"

int	main () {
	server	irc( 6667 );

	irc.startServer();
	irc.runServerLoop();
}