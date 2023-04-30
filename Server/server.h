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
const int BOARD_SIZE = 15;
const int NUM_SHIPS = 9; // 1 portaaviones, 2 buques, 2 submarinos, 3 lanchas
// * Portaaviones:
// ! 5 posiciones

// * Buque:
// ! 4 posiciones

// * Submarino:
// ! 3 posiciones

// * Lancha:
// ! 1 posiciones

// representacion de una posicion en el tablero
struct Position
{
    int x;
    int y;
};

// representacion de un barco
struct Ship
{
    std::vector<Position> positions;
    char type; // 'P' = portaaviones, 'B' = buque, 'S' = submarino, 'L' = lancha
};

// representacion de un tablero
struct Board
{
    char board[BOARD_SIZE][BOARD_SIZE];
    std::vector<Ship> ships;
};

// representacion de un jugador
struct Player
{
    int id;
    int socket_fd;

};

// representacion de un juego
struct Game
{
    int id; // identificador del juego
    Player player;
    int turn; // 0 = no ha empezado, 1 = turno del jugador 1, 2 = turno del servidor
    bool is_game_over;
    bool is_player_ready;
};


// variables globales
std::vector<Game> games;
std::mutex games_mutex;
std::condition_variable games_cv;

// funciones
int find_port(int initial_port);

int find_port(int initial_port){
    int port = initial_port;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error al crear el socket del servidor" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port=htons(port);

    // intenta vincular el socket a la direccion del servidor
    int bind_result = bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    while (bind_result < 0)
    {
        port++;
        server_address.sin_port = htons(port);
        bind_result = bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    }

    // cuando se logra vincular el socket a la direccion del servidor, se cierra el socket
    // y se retorna el puerto para abrir el socket del servidor en el main
    close(sockfd);
    return port;
}

void generateBoard(Board& board)
{
    // Inicializar el tablero con 'O'
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board.board[i][j] = 'O';
        }
    }

    // Generar los barcos aleatoriamente
    srand(time(nullptr));

    // Barco tipo Portaaviones
    for (int i = 0; i < 1; i++)
    {
        Ship ship;
        ship.type = 'P';
        ship.positions.resize(5);
        for (int j = 0; j < 5; j++)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            ship.positions[j] = {x, y};
            board.board[x][y] = ship.type;
        }
        board.ships.push_back(ship);
    }

    // Barcos tipo Buque
    for (int i = 0; i < 2; i++)
    {
        Ship ship;
        ship.type = 'B';
        ship.positions.resize(4);
        for (int j = 0; j < 4; j++)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            ship.positions[j] = {x, y};
            board.board[x][y] = ship.type;
        }
        board.ships.push_back(ship);
    }

    // Barcos tipo Submarino
    for (int i = 0; i < 2; i++)
    {
        Ship ship;
        ship.type = 'S';
        ship.positions.resize(3);
        for (int j = 0; j < 3; j++)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            ship.positions[j] = {x, y};
            board.board[x][y] = ship.type;
        }
        board.ships.push_back(ship);
    }

    // Barcos tipo Lancha
    for (int i = 0; i < 3; i++)
    {
        Ship ship;
        ship.type = 'L';
        ship.positions.resize(1);
        for (int j = 0; j < 1; j++)
        {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            ship.positions[j] = {x, y};
            board.board[x][y] = ship.type;
        }
        board.ships.push_back(ship);
    }
}


