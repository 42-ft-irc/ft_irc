#include "server.hpp"

bool isRunning = true;

void signalHandler(int signum) {
	(void)signum; // Unused parameter warning vermeiden
	std::cout << "\nSignal received, shutting down..." << std::endl;
	isRunning = false;
}

int	main () {
	signal(SIGINT, signalHandler);
	try {
		server	irc( PORT );
		irc.startServer();
		irc.runServerLoop();
	}
	catch (std::exception &e) { 
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}