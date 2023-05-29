CC = g++
CFLAGS = -std=c++11

CLIENT_SRCS = Client/client.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
CLIENT_TARGET = client

SERVER_SRCS = Server/server.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
SERVER_TARGET = server

COMMON_HEADERS = Client/class.h Client/game.h Client/libraries.h Client/listener.h Client/messages.h \
                 Server/board.h Server/game.h Server/libraries.h Server/player.h Server/position.h Server/server.h Server/ship.h

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(CLIENT_OBJS) -o $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(SERVER_OBJS) -o $(SERVER_TARGET)

$(CLIENT_OBJS): %.o : %.cpp $(COMMON_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(SERVER_OBJS): %.o : %.cpp $(COMMON_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)
