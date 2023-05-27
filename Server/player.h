#ifndef PLAYER_H
#define PLAYER_H
// clase que representa un jugador
class Player
{
public:
    int id;
    int socket_fd;

    Player(int id, int socket_fd)
    {
        this->id = id;
        this->socket_fd = socket_fd;
    }

    Player()
    {
        this->id = -1;
        this->socket_fd = -1;
    }
};

#endif // PLAYER_H