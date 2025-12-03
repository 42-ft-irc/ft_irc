#include "server.hpp"

bool isRunning = true;

void signalHandler(int signum) {
	(void)signum;
	std::cout << "\nSignal received, shutting down..." << std::endl;
	isRunning = false;
}

int	main (int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	int port = atoi(argv[1]);
	if (port <= 0 || port > 65535) {
		std::cerr << "Error: Invalid port number" << std::endl;
		return 1;
	}

	std::string password = argv[2];
	if (password.empty()) {
		std::cerr << "Error: Password cannot be empty" << std::endl;
		return 1;
	}

	signal(SIGINT, signalHandler);
	try {
		server	irc(port, password);
		irc.startServer();
		irc.runServerLoop();
	}
	catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}