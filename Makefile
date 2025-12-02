CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SERVER_NAME = server
CLIENT_NAME = client

SERVER_DIR = server-proto
CLIENT_DIR = client-proto

SERVER_SRCS = $(SERVER_DIR)/server.cpp $(SERVER_DIR)/main.cpp
CLIENT_SRCS = $(CLIENT_DIR)/client.cpp $(CLIENT_DIR)/util.cpp

SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME)

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_NAME)

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS)

fclean: clean
	rm -f $(SERVER_NAME) $(CLIENT_NAME)

re: fclean all

.PHONY: all clean fclean re
