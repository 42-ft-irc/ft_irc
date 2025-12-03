#include "server.hpp"

int	main () {
	try {
		server	irc( 6667 );
		irc.startServer();
		irc.runServerLoop();
	}
	catch (std::exception &e) { 
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}