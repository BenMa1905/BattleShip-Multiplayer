#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <condition_variable>


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
// representacion de un tablero
struct Board
{
    std::vector<std::vector<char>> grid;
    std::vector<Ship> ships;
};


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
const char SHIP_TYPES[] = {'P', 'B', 'B', 'S', 'S', 'L', 'L', 'L'};

// funciones
Board create_board();
Ship create_ship(char type, int size);
void print_board(Board board);
std::string board_to_string(Board board);
char receive_shot(int client_sockfd);
char send_shot(std::string input, int client_sockfd);
char get_letter(int number);
bool is_valid_position(const Position &position);
bool is_positon_available(const Board &board, const Position &position);
bool is_ship_position_valid(const Board &board, const Ship &ship);
void add_ship_to_board(Board &board, Ship &ship);
int is_valid_coord(std::string coord);
char manage_game(int client_sockfd, char lastShot);

// ! Definicion de colores para imprimir
const std::string ANSI_RESET = "\u001b[0m";
const std::string ANSI_BLACK = "\u001b[30m";
const std::string ANSI_RED = "\u001b[31m";
const std::string ANSI_GREEN = "\u001b[32m";
const std::string ANSI_YELLOW = "\u001b[33m";
const std::string ANSI_BLUE = "\u001b[34m";
const std::string ANSI_MAGENTA = "\u001b[35m";
const std::string ANSI_CYAN = "\u001b[36m";
const std::string ANSI_WHITE = "\u001b[37m";


/*
    !create_board
    * Crea un tablero de tamaño BOARD_SIZE x BOARD_SIZE
    * con los barcos en posiciones aleatorias
    *
    * @brief Función para crear un tablero vacío (O = agua)
    * @return Tablero vacío
*/
Board create_board()
{
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

/*
    !board_to_string
    * Convierte un tablero a un string
*/

std::string board_to_string(Board board)
{
    std::string board_string = "";
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        board_string += std::string(board.grid[i].begin(), board.grid[i].end());
    }
    return board_string;
}

// ! printBoard
// Función para imprimir las tablas
void printBoard(Board board)
{
    // Imprimir los números de las columnas
    std::cout << "   ";
    for (int col = 0; col < BOARD_SIZE; col++)
    {
        std::cout << std::setw(3) << col + 1;
    }
    std::cout << "        ";
    for (int col = 0; col < BOARD_SIZE; col++)
    {
        std::cout << std::setw(3) << col + 1;
    }
    std::cout << std::endl;

    // Imprimir las tablas
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        // Imprimir la letra de la fila
        std::cout << " " << static_cast<char>('A' + row) << " ";

        // Imprimir la tabla del cliente
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            char cell = clientBoard[row][col];
            if (cell == 'O')
            {
                cell = REPLACEMENT_CHAR;
            }
            std::cout << "\033[1;34m" << std::setw(3) << "\033[0m" << cell;
        }

        // Separación entre las dos tablas
        std::cout << "      ";

        // Imprimir la tabla del servidor
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            char cell = serverBoard[row][col];
            if (cell == 'O')
            {
                cell = REPLACEMENT_CHAR;
            }
            std::cout << "\033[1;34m" << std::setw(3) << "\033[0m" << cell;
        }

        std::cout << std::endl;
    }
}

/*
    ! send_shoot
    * Función para disparar a una coordenada
*/
char send_shot(std::string input, int client_sockfd)
{
    char result;
    std::stringstream ss;
    int coord;

    if (input.length() == 3)
    {
        coord = (input[1] * 10) + input[2];
    }
    else
    {
        coord = input[1];
    }
    ss << "*," << input[0] << "," << coord;
    std::string message = ss.str();

    // Se envía la letra al servidor
    if (send(client_sockfd, message.c_str(), message.length(), 0) < 0)
    {
        cerr << "Error al enviar el disparo al servidor" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "Disparando a la coordenada: (" << input[0] << ", " << coord << ")" << endl;
        // Se recibe la respuesta del servidor
        result = receive_shot(client_sockfd);
    }
    return (result);
}

/*
    ! receive_shoot
    * Función para recibir un disparo
*/
char receive_shot(int client_sockfd)
{
    std::string result; // Mensaje que llega desde server
    char box;           // donde se guardará el resultado del disparo
    int coordY, coordX; // coordenadas del disparo
    char auxChar;       // caracter auxiliar para el retorno de la función

    if (recv(client_sockfd, &result, sizeof(result), 0) < 0)
    {
        cerr << "Error al recibir el resultado del servidor" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::istringstream iss(result);
        iss >> box;
        iss.ignore(); // Ignorar la coma
        iss >> coordY;
        iss.ignore(); // Ignorar la coma
        iss >> coordY;
        cout << ANSI_RED << "Te han disparado a " << get_letter(coordX) << coordY << ANSI_RESET << endl;

        if (clientBoard[coordY][coordX] == 'O')
        {
            cout << ANSI_GREEN << "El servidor ha fallado!" << ANSI_RESET << endl;
            clientBoard[coordY][coordX] = 'X';
            return 'X';
        }
        else
        {
            auxChar = clientBoard[coordY][coordX];
            clientBoard[coordY][coordX] = 'X';
            return auxChar;
        }
    }
    return 'E';
}

// ! get_letter
// Función para obtener la letra de la coordenada en el eje Y
char get_letter(int number)
{
    char letter = 'A' + number - 1;
    return letter;
}

// ! createShip
Ship create_ship(char type, int size)
{
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

/*
    ! is_valid_position
    * Determina si una posición es válida dentro del tablero
    *
    * @brief Función para determinar si una posición es válida
    * @param position Posición a evaluar
    * @return true si la posición es válida, false en caso contrario
 */
bool is_valid_position(const Position &position)
{
    return position.x >= 0 && position.x < BOARD_SIZE && position.y >= 0 && position.y < BOARD_SIZE;
}

/*
    ! is_positon_available
    * Determina si una posición está disponible en el tablero
    * @brief Función para determinar si una posición está disponible
    * @param board Tablero
    * @param position Posición a evaluar
    * @return true si la posición está disponible, false en caso contrario
 */
bool is_positon_available(const Board &board, const Position &position)
{
    for (const Ship &ship : board.ships)
    {
        for (const Position &p : ship.positions)
        {
            if (p.x == position.x && p.y == position.y)
            {
                return false;
            }
        }
    }
    return true;
}

/*
    ! is_ship_position_valid
    * Determina si una posición es válida y está disponible en el tablero
    * @brief Función para determinar si una posición es válida y está disponible
    * @param board Tablero
    * @param position Posición a evaluar
    * @return true si la posición es válida y está disponible, false en caso contrario
 */
bool is_ship_position_valid(const Board &board, const Ship &ship)
{
    for (const Position &position : ship.positions)
    {
        if (!is_valid_position(position) || !is_positon_available(board, position))
        {
            return false;
        }
    }
    return true;
}

/*
    ! add_ship_to_board
    * Agrega un barco al tablero
    *
    * @brief Función para agregar un barco al tablero
    * @param board Tablero
    * @param ship Barco a agregar
 */
void add_ship_to_board(Board &board, Ship &ship)
{
    for (const Position &position : ship.positions)
    {
        board.grid[position.x][position.y] = ship.type;
    }
}

/*
! is_valid_coord
* Determina si una coordenada es válida
* devuelve un 0, si no es válida
* devuelve un 1, si es válida
* devuelve un 2, si es la palabra exit, lo que hace que el programa termine
*/
int is_valid_coord(std::string coord)
{
    if (coord == "exit")
    {
        return 2;
    }

    switch (coord.length())
    {
    case 2:
        if (!isalpha(coord[0]) || !isdigit(coord[1]))
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    case 3:
        int num = (coord[1] * 10) + coord[2];
        if (!isalpha(coord[0]) || num > 15 || num < 1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
        break;
    default:
        return 0;
        break;
    }

    return 1;
}

/*
    ! manage_game
    * Función para manejar el juego
    * maneja las funciones de disparo y de recibir disparo
*/
char manage_game(int client_sockfd, char lastShot)
{
    /*
    TODO: Implementar respuestas de los barcos
    *revisar respuestas y envios de disparos
    */
    // * Variables
    std::string input;

    // * Loop principal
    do
    {
        print_board(clientBoard);// Imprimir el tablero
        cout << "Elige la coordenada a la que disparar" << endl;
        cin >> input;// * se le pide al usuario que ingrese la coordenada
        // Verificaciones del string con la funcion is_valid_coord
        switch (is_valid_coord(input))
        {
        case 0: // si es 0 es invalido
            std::cout << "Coordenada inválida." << std::endl;
            break;
        case 1: // si es 1 es valido
            send_shot(input, client_sockfd);
            break;
        case 2: // si es 2 es exit
            return false;
            break;
        default:
            break;
        }
        // Verificar que el string tenga exactamente 2 caracteres
    } while (true);
    // * Fin del loop principal
}
