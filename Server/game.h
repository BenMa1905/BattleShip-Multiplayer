#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>


// constantes del juego
const int BOARD_SIZE = 15;
const int NUM_SHIPS = 9; // 1 portaaviones, 2 buques, 2 submarinos, 3 lanchas
const char SHIP_TYPES[] = {'P', 'B', 'B', 'S', 'S', 'L', 'L', 'L'};

/*
    Se empieza del 0 el tablero
    hay que mandar solo a lo que le dio cuando dispara
    el jugador manda la posicion en la que quiere disparar y el servidor le responde si le dio a algo o no
    O = agua, X = hundido
*/

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
    char type; // 'P' = portaaviones, 'B' = buque, 'S' = submarino, 'L' = lancha, 'X' = hundido, 'O' = agua
};

// representacion de un jugador
struct Player
{
    int id;
    int socket_fd;
};

// representacion de un tablero
struct Board
{
    std::vector<std::vector<char>> grid;
    std::vector<Ship> ships;
};

// representacion de un juego
struct Game
{
    int id; // identificador del juego
    Player player;
    int turn; // 0 = no ha empezado, 1 = turno del jugador 1, 2 = turno del servidor
    bool is_game_over;
    // bool is_player_ready;
    Board player_board;
    Board server_board;
};


// funciones
Board create_board();
Board text_to_board(std::string text);
void print_board(const Board& board);


// funciones auxiliares para crear los barcos
// el const impide que se modifique el parametro
// el & indica que se pasa por referencia (no se hace una copia)
Ship create_ship(char type, int size);
bool is_valid_position(const Position& position);
bool is_positon_available(const Board& board,const Position& position);
bool is_ship_position_valid(const Board& board,const Ship& ship);
void add_ship_to_board(Board& board, Ship& ship);

/**
 * funcion para imprimir un tablero
 * @param board tablero a imprimir
 * @return void
*/
void print_board(const Board& board){
    std::cout << "    ";
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        if(i < 10){
            std::cout << i << "  ";
        }else{
            std::cout << i<< " ";
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        // agregar un espacio antes de los numeros menores a 10 para que se vea mejor
        if(i < 10){
            std::cout << i << "   ";
        }else{
            std::cout << i << "  ";
        }

        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if(board.grid[i][j] == 'X'){
                std::cout << "\033[1;31m" << board.grid[i][j] << "\033[0m  ";
            }else if(board.grid[i][j] == 'O'){
                std::cout << "\033[1;34m" << board.grid[i][j] << "\033[0m  ";
            }else{
                std::cout << board.grid[i][j] << "  ";
            }
            
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
 * transforma texto a un tablero de 15x15
 * el texto debe ir con el formato de 15 filas de 15 caracteres separados por comas
 * "OOOOOOOOOOOOOOO,OOOOOOOOOOOOOOO,..."
 * @param text texto a transformar
 * @return tablero de 15x15
*/
Board text_to_board(std::string text){
    Board board;
    board.grid.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board.grid[i].resize(BOARD_SIZE);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board.grid[i][j] = text[i*BOARD_SIZE + j];
        }
    }

    // el texto viene con el siguiente formato: OOOOOOOOOOOOOOO,OOOOOOOOOOOOOOO, asi hasta 15 filas
    // se debe convertir a una matriz de 15x15
    std::string lines[BOARD_SIZE];
    int start = 0;
    int end = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        end = text.find(",", start);
        lines[i] = text.substr(start, end - start);
        start = end + 1;
    }

    // se llena el tablero con los datos del texto
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board.grid[i].resize(BOARD_SIZE);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board.grid[i][j] = lines[i][j];
        }
    }

    // impresion del tablero para verificar que se haya llenado correctamente
    std::cout << "user board: " << std::endl;
    print_board(board);

    return board;
}


/**
* Crea un tablero de tamaño BOARD_SIZE x BOARD_SIZE
* con los barcos en posiciones aleatorias
*
* @brief Función para crear un tablero vacío (O = agua)
* @return Tablero vacío
*/
Board create_board(){
    Board board;
    board.grid.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board.grid[i].resize(BOARD_SIZE);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board.grid[i][j] = 'O';
        }
    }

    for (char type : SHIP_TYPES)
    {
        int size = 0;
        switch (type)
        {
            case 'P':
                size = 5;
                break;
            case 'B':
                size = 4;
                break;
            case 'S':
                size = 3;
                break;
            case 'L':
                size = 1;
                break;
        }

        for (int i = 0; i < 100; i++) // intenta 100 veces crear un barco válido
        {
            Ship ship = create_ship(type, size);

            if (is_ship_position_valid(board, ship))
            {
                add_ship_to_board(board, ship);
                break;
            }
        }
    }

    std::cout << "board created: " << std::endl;
    print_board(board);

    return board;
}

/**
 * Crea los barcos del tablero con posiciones aleatorias
 * dentro de los límites del tablero y sin que se superpongan
 * 
 * @brief Función para crear los barcos del tablero
 * @return Vector de barcos
 */
Ship create_ship(char type, int size){
    Ship ship;
    ship.type = type;
    int orientation = std::rand() % 2; // 0 = horizontal, 1 = vertical

    Position start_pos;
    start_pos.x = std::rand() % BOARD_SIZE;
    start_pos.y = std::rand() % BOARD_SIZE;

    ship.positions.push_back(start_pos);

    for (int i = 1; i < size; i++)
    {
        Position pos = start_pos;

        if (orientation == 0) // horizontal
        {
            pos.y += i;
        }
        else // vertical
        {
            pos.x += i;
        }

        ship.positions.push_back(pos);
    }

    return ship;
}

/**
 * Determina si una posición es válida dentro del tablero
 * 
 * @brief Función para determinar si una posición es válida
 * @param position Posición a evaluar
 * @return true si la posición es válida, false en caso contrario
*/
bool is_valid_position(const Position& position){
    return position.x >= 0 && position.x < BOARD_SIZE && position.y >= 0 && position.y < BOARD_SIZE;
}


/**
 * Determina si una posición está disponible en el tablero
 * 
 * @brief Función para determinar si una posición está disponible
 * @param board Tablero
 * @param position Posición a evaluar
 * @return true si la posición está disponible, false en caso contrario
*/
bool is_positon_available(const Board& board,const Position& position){
    for (const Ship& ship : board.ships)
    {
        for (const Position& p : ship.positions)
        {
            if (p.x == position.x && p.y == position.y)
            {
                return false;
            }
        }
    }
    return true;
}

/**
 * Determina si una posición es válida y está disponible en el tablero
 * 
 * @brief Función para determinar si una posición es válida y está disponible
 * @param board Tablero
 * @param position Posición a evaluar
 * @return true si la posición es válida y está disponible, false en caso contrario
*/
bool is_ship_position_valid(const Board& board,const Ship& ship){
    for (const Position& position : ship.positions)
    {
        if (!is_valid_position(position) || !is_positon_available(board, position))
        {
            return false;
        }
    }
    return true;
}


/**
 * Agrega un barco al tablero
 * 
 * @brief Función para agregar un barco al tablero
 * @param board Tablero
 * @param ship Barco a agregar
*/
void add_ship_to_board(Board& board, Ship& ship){
    for (const Position& position : ship.positions)
    {
        board.grid[position.x][position.y] = ship.type;
    }
}




