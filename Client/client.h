#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <condition_variable>

// constantes del juego
const char REPLACEMENT_CHAR = '-';
// Caracter para reemplazar las 'O'
// Tamaño del tablero
const int BOARD_SIZE = 15;

// Tableros del cliente y del servidor
char clientBoard[BOARD_SIZE][BOARD_SIZE];
char serverBoard[BOARD_SIZE][BOARD_SIZE];
// Número de barcos
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

int find_port(int initial_port)
{
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
    server_address.sin_port = htons(port);

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

// ! printBoard
// Función para imprimir las tablas
void printBoard(char clientBoard[BOARD_SIZE][BOARD_SIZE], char serverBoard[BOARD_SIZE][BOARD_SIZE]) {
    // Imprimir los números de las columnas
    std::cout << "   ";
    for (int col = 0; col < BOARD_SIZE; col++) {
        std::cout << std::setw(3) << col + 1;
    }
    std::cout << "        ";
    for (int col = 0; col < BOARD_SIZE; col++) {
        std::cout << std::setw(3) << col + 1;
    }
    std::cout << std::endl;

    // Imprimir las tablas
    for (int row = 0; row < BOARD_SIZE; row++) {
        // Imprimir la letra de la fila
        std::cout << " " << static_cast<char>('A' + row) << " ";

        // Imprimir la tabla del cliente
        for (int col = 0; col < BOARD_SIZE; col++) {
            char cell = clientBoard[row][col];
            if (cell == 'O') {
                cell = REPLACEMENT_CHAR;
            }
            std::cout << std::setw(3) << cell;
        }

        // Separación entre las dos tablas
        std::cout << "      ";

        // Imprimir la tabla del servidor
        for (int col = 0; col < BOARD_SIZE; col++) {
            char cell = serverBoard[row][col];
            if (cell == 'O') {
                cell = REPLACEMENT_CHAR;
            }
            std::cout << std::setw(3) << cell;
        }

        std::cout << std::endl;
    }
}

// ! shoot
char shoot(std::string input, int client_sockfd)
{
    char resultado;
    // Se envía la letra al servidor
    if (send(client_sockfd, input.c_str(), input.length(), 0) < 0)
    {
        cerr << "Error al enviar el disparo al servidor" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "Disparando a la coordenada: " << input << endl;
        // Se recibe la respuesta del servidor
        if (recv(client_sockfd, &resultado, sizeof(resultado), 0) < 0)
        {
            cerr << "Error al recibir el resultado del servidor" << endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            // ! Booleano para saber si se termino el juego
            cout << "Le has dado a un barco enemigo: " << resultado << endl;
        }
    }
    return (resultado);
}
