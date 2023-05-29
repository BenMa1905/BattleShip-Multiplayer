#ifndef BOARD_H
#define BOARD_H

// #include <sstream>
// #include <vector>

#include "ship.h"
#include "server.h"

/*
    Se empieza del 0 el tablero
    hay que mandar solo a lo que le dio cuando dispara
    el jugador manda la posicion en la que quiere disparar y el servidor le responde si le dio a algo o no
    O = agua, X = hundido
*/
// clase que representa un tablero
class Board
{
public:
    std::vector<std::vector<char>> grid;
    std::vector<Ship> ships;

    Ship createShip(char type, int size, const Board &board);
    /**
     * Crea un tablero de tamaño BOARD_SIZE x BOARD_SIZE
     * vacío (O = agua)
     *
     * @brief Función para crear un tablero vacío (O = agua)
     * @return Tablero vacío
     */
    Board()
    {
        // inicializar el tablero con agua en todas las posiciones
        this->grid = std::vector<std::vector<char>>(BOARD_SIZE, std::vector<char>(BOARD_SIZE, 'O'));
        this->ships = std::vector<Ship>(NUM_SHIPS);
    }

    /**
     * Funcion que comprueba que no queden barcos en el tablero
     * si no quedan barcos retorna true, si quedan retorna false
     * @param board tablero en el que se va a comprobar
     * @return bool true si no quedan barcos, false si quedan
     */
    bool checkGameOver() const
    {
        for (const Ship &ship : this->ships)
        {
            if (!ship.isSunk())
            {
                // std::cout << "No se ha hundido el barco " << ship.type << std::endl;
                return false;
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
    bool isShipPositionValid(const Ship &ship) const
    {

        for (const Position &position : ship.positions)
        {
            if (!position.isValidPosition() || !this->isPositonAvailable(position))
            {
                return false;
            }
        }

        return true;
    }

    /**
     * Imprime los barcos del tablero
     * @param board Tablero
     * @return void
     */
    void printShips() const
    {
        for (const Ship &ship : this->ships)
        {
            std::cout << "Barco " << ship.type << " hundido: " << ship.isSunk() << std::endl;
        }
    }

    /**
     * Agrega un barco al tablero
     *
     * @brief Función para agregar un barco al tablero
     * @param board Tablero
     * @param ship Barco a agregar
     */
    void addShipToBoard(Ship &ship)
    {
        for (const Position &position : ship.positions)
        {
            this->grid[position.x][position.y] = ship.type;
        }

        this->ships.push_back(ship);
    }

    /**
     * funcion que recibe un disparo y retorna a que le dio
     * @param x posicion x del disparo
     * @param y posicion y del disparo
     * @return char 'O' si no le dio a nada, 'P' si le dio a un portaaviones, 'B' si le dio a un buque, 'S' si le dio a un submarino, 'L' si le dio a una lancha
     */
    char checkShot(int x, int y)
    {
        char response;
        if (grid[x][y] == 'O')
        {
            response = 'O';
            grid[x][y] = 'X';
        }
        else
        {

            response = grid[x][y];
            // actualizar el valor de hits del barco, buscar el barco que corresponde
            // buscar la posicion en la que le dio, y actualizar el valor de hits de esa posicion
            for (Ship &ship : this->ships)
            {
                if (response != ship.type)
                {
                    continue;
                }
                for (size_t i = 0; i < ship.positions.size(); i++)
                {
                    if (ship.positions[i].x == x && ship.positions[i].y == y)
                    {
                        // Se encontró la posición, actualizar el valor de hits
                        ship.hits[i] = true;
                        break; // Terminar el bucle si se encontró la posición
                    }
                }
            }

            grid[x][y] = 'X';
        }
        return response;
    }

    /**
     * funcion que retorna el ship que se encuentra en la posicion x,y
     * @param x posicion x del disparo
     * @param y posicion y del disparo
     * @return Ship ship que se encuentra en la posicion x,y
     */
    Ship getShip(int x, int y)
    {
        // char response = grid[x][y];
        Ship ship;
        for (Ship &ship : this->ships)
        {
            for (size_t i = 0; i < ship.positions.size(); i++)
            {
                if (ship.positions[i].x == x && ship.positions[i].y == y)
                {
                    // Se encontró la posición, actualizar el valor de hits
                    // ship.hits[i] = true;
                    return ship;
                    break; // Terminar el bucle si se encontró la posición
                }
            }
        }
        return ship;
    }

    /**
     * Determina si una posición está disponible en el tablero
     *
     * @brief Función para determinar si una posición está disponible
     * @param board Tablero
     * @param position Posición a evaluar
     * @return true si la posición está disponible, false en caso contrario
     */
    bool isPositonAvailable(const Position &position) const
    {
        if (this->ships.empty())
        {
            return true;
        }
        for (const Ship &ship : this->ships)
        {
            for (const Position &ship_position : ship.positions)
            {
                if (ship_position.x == position.x && ship_position.y == position.y)
                {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * funcion para imprimir un tablero
     * @param board tablero a imprimir
     * @return void
     */
    void printBoard() const
    {
        std::cout << "    ";
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            if (i < 10)
            {
                std::cout << i << "  ";
            }
            else
            {
                std::cout << i << " ";
            }
        }
        std::cout << std::endl;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            // agregar un espacio antes de los numeros menores a 10 para que se vea mejor
            if (i < 10)
            {
                std::cout << i << "   ";
            }
            else
            {
                std::cout << i << "  ";
            }

            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (grid[i][j] == 'X')
                {
                    std::cout << "\033[1;31m" << grid[i][j] << "\033[0m  ";
                }
                else if (grid[i][j] == 'O')
                {
                    std::cout << "\033[1;34m" << grid[i][j] << "\033[0m  ";
                }
                else
                {
                    std::cout << grid[i][j] << "  ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    /**
     * Crea un tablero de tamaño BOARD_SIZE x BOARD_SIZE
     * con los barcos en posiciones aleatorias
     *
     * @brief Función para crear un tablero vacío (O = agua)
     * @return Tablero vacío
     */
    Board createRandomBoard()
    {
        Board board;

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
                // std::srand(std::time(nullptr));
                // std::cout << "creating ship: " << type << " i: " << i << std::endl;
                Ship ship = createShip(type, size, board);

                // for (int j = 0; j < size; j++)
                // {
                //     // std::cout << "ship position: " << ship.positions[j].x << ", " << ship.positions[j].y << std::endl;
                // }

                if (board.isShipPositionValid(ship))
                {
                    board.addShipToBoard(ship);
                    // std::cout << "ship created: " << type << " at i: " << i << std::endl;
                    break;
                }
            }
        }

        // std::cout << "board created: " << std::endl;
        // board.printBoard();

        return board;
    }
};

/**
 * Función para encontrar las posiciones de un barco en el tablero
 * @param row fila actual
 * @param col columna actual
 * @param type tipo de barco
 * @param board tablero
 * @param visited matriz de visitados
 * @param positions vector de posiciones
 * @return void
 */
void findShipPositions(int row, int col, char type, const Board &board, std::vector<std::vector<bool>> &visited, std::vector<Position> &positions)
{
    visited[row][col] = true;
    positions.push_back(Position(row, col));

    if (row > 0 && !visited[row - 1][col] && board.grid[row - 1][col] == type)
    {
        findShipPositions(row - 1, col, type, board, visited, positions);
    }
    if (row < BOARD_SIZE - 1 && !visited[row + 1][col] && board.grid[row + 1][col] == type)
    {
        findShipPositions(row + 1, col, type, board, visited, positions);
    }
    if (col > 0 && !visited[row][col - 1] && board.grid[row][col - 1] == type)
    {
        findShipPositions(row, col - 1, type, board, visited, positions);
    }
    if (col < BOARD_SIZE - 1 && !visited[row][col + 1] && board.grid[row][col + 1] == type)
    {
        findShipPositions(row, col + 1, type, board, visited, positions);
    }
}

/**
 * transforma texto a un tablero de 15x15
 * el texto debe ir con el formato de 15 filas de 15 caracteres separados por comas
 * "OOOOOOOOOOOOOOO,OOOOOOOOOOOOOOO,..."
 * @param text texto a transformar
 * @return tablero de 15x15
 */
Board textToBoard(std::string text)
{
    Board board = Board();

    // separar el texto por comas
    std::vector<std::string> rows = split(text, ',');
    if (rows.size() != BOARD_SIZE)
    {
        throw std::runtime_error("invalid board size");
    }

    // recorrer cada fila
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        std::string row = rows[i];

        // recorrer cada caracter de la fila
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            char c = row[j];
            if (c != 'O' && c != 'P' && c != 'B' && c != 'S' && c != 'L')
            {
                throw std::runtime_error("invalid character");
            }
            board.grid[i][j] = c;
        }
    }

    // std::cout << "board created: " << std::endl;
    // board.printBoard();

    std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

    board.ships.clear();
    // Buscar barcos en el tablero y agrupar las posiciones del mismo barco
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (!visited[i][j])
            {
                char c = board.grid[i][j];
                if (c != 'O' && c != 'X')
                {
                    // Se encontró un barco, se crea un objeto Ship y se agregan las posiciones del mismo barco
                    Ship ship = Ship(c);
                    findShipPositions(i, j, c, board, visited, ship.positions);
                    board.ships.push_back(ship);
                }
            }
        }
    }

    // std::cout << "ships on board: " << std::endl;
    // for (Ship ship : board.ships)
    // {
    //     std::cout << ship.type << std::endl;
    // }

    return board;
}

/**
 * Crea los barcos del tablero con posiciones aleatorias
 * dentro de los límites del tablero y sin que se superpongan
 *
 * @brief Función para crear los barcos del tablero
 * @return Vector de barcos
 */
Ship Board::createShip(char type, int size, const Board &board)
{
    Ship ship(type, size);
    int orientation = std::rand() % 2; // 0 = horizontal, 1 = vertical

    while (true)
    {
        Position start_pos = randomPosition();
        ship.positions[0] = start_pos;

        Position pos = start_pos;
        bool validPosition = true;

        for (int i = 1; i < size; i++)
        {
            if (orientation == 0) // horizontal
            {
                pos.x += 1;
            }
            else // vertical
            {
                pos.y += 1;
            }

            if (!pos.isValidPosition() || board.isShipPositionValid(ship))
            {
                validPosition = false;
                break;
            }

            ship.positions[i] = pos;
        }

        if (validPosition)
        {
            break;
        }
    }

    return ship;
}

#endif // BOARD_H