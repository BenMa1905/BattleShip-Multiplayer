CC = g++
CFLAGS = -std=c++11 -Wall

SERVER_DIR = Server
SERVER_SRCS = $(SERVER_DIR)/server.cpp $(SERVER_DIR)/board.cpp $(SERVER_DIR)/game.cpp $(SERVER_DIR)/player.cpp $(SERVER_DIR)/position.cpp $(SERVER_DIR)/ship.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
SERVER_TARGET = server

all: $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $(SERVER_TARGET)

$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_DIR)/*.o $(SERVER_TARGET)

