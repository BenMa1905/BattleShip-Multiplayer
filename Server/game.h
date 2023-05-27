#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

// clase que representa un juego
class Game
{
public:
    pid_t id; // identificador del juego
    Player player;
    Board player_board;
    Board server_board;

    Game(pid_t id, Player player, Board playerBoard, Board serverBoard)
    {
        this->id = id;
        this->player = player; // jugador que se une al juego -1 (valor por defecto o no se ha unido nadie)
        this->player_board = playerBoard;
        this->server_board = serverBoard;
    }

    
};

#endif // GAME_H

