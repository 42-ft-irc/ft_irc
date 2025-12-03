CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I server-proto/includes/

SERVER_NAME = server
CLIENT_NAME = client

SERVER_DIR = server-proto/srcs
CLIENT_DIR = client-proto
OBJS_DIR = server-proto/objs

SERVER_SRCS = $(shell find $(SERVER_DIR) -name '*.cpp')
CLIENT_SRCS = $(CLIENT_DIR)/client.cpp $(CLIENT_DIR)/util.cpp

SERVER_OBJS = $(SERVER_SRCS:$(SERVER_DIR)/%.cpp=$(OBJS_DIR)/%.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

all: $(SERVER_NAME) $(CLIENT_NAME)

$(OBJS_DIR)/%.o: $(SERVER_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME) $(INCLUDES)

$(CLIENT_NAME): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_NAME)

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS)

fclean: clean
	rm -f $(SERVER_NAME) $(CLIENT_NAME)

re: fclean all

.PHONY: all clean fclean re
