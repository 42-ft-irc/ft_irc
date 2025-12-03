#include "server.hpp"

int	main () {
	server	irc( 6667 );

	irc.startServer();
	try {
		irc.runServerLoop();
	}
	catch (std::exception &e) { 
		std::cerr << "Error: " << e.what() << std::endl;
	}
}