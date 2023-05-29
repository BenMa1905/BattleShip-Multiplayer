#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
// clase que representa un jugador
class Player
{
public:
    int id;
    int socket_fd;
    char client_ip[INET_ADDRSTRLEN];
    int client_port;

    Player(int id, int socket_fd, char client_ip[INET_ADDRSTRLEN], int client_port)
    {
        this->id = id;
        this->socket_fd = socket_fd;
        strcpy(this->client_ip, client_ip);
        this->client_port = client_port;
    }

    Player()
    {
        this->id = -1;
        this->socket_fd = -1;
        strcpy(this->client_ip, "");
        this->client_port = -1;
    }
};

#endif // PLAYER_H