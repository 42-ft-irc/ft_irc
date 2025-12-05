CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I includes/

SERVER_NAME = ircserv

SERVER_DIR = srcs
OBJS_DIR = objs

SERVER_SRCS = $(shell find $(SERVER_DIR) -name '*.cpp')

SERVER_OBJS = $(SERVER_SRCS:$(SERVER_DIR)/%.cpp=$(OBJS_DIR)/%.o)

all: $(SERVER_NAME)

$(OBJS_DIR)/%.o: $(SERVER_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(SERVER_NAME): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_NAME) $(INCLUDES)

clean:
	rm -f $(SERVER_OBJS)

fclean: clean
	rm -f $(SERVER_NAME)

re: fclean all

.PHONY: all clean fclean re
