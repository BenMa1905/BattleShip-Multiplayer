#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

// constantes del juego
const BOARD_SIZE = 15;
const int NUM_SHIPS = 9; // 1 portaaviones, 2 buques, 2 submarinos, 3 lanchas



// representacion de una posicion en el tablero
struct Position {
    int x;
    int y;
};

// representacion de un barco
struct Ship {
    std::vector<Position> positions;
    char type; // 'P' = portaaviones, 'B' = buque, 'S' = submarino, 'L' = lancha
}

// representacion de un jugador
struct Player {
    int id;
    int socket_fd;
}

// representacion de un juego
struct Game
{
    int id; // identificador del juego
    Player player;
    int turn; // 0 = no ha empezado, 1 = turno del jugador 1, 2 = turno del servidor
    bool is_game_over;
    bool is_player_ready;
};


