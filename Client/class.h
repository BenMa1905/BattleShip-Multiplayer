#ifndef CLASS_H
#define CLASS_H

#include <vector>
#include <iostream>
#include <iomanip>

const int BOARD_SIZE = 15;
const std::vector<char> SHIP_TYPES = {'P', 'B', 'B', 'S', 'S', 'L', 'L', 'L'};

class Position
{
public:
    int x;
    int y;

    Position(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

class Ship
{
public:
    std::vector<Position> positions;
    char type;

    Ship(char type)
    {
        this->type = type;
    }
};

class Board
{
public:
    std::vector<std::vector<char>> grid;
    std::vector<Ship> ships;

    Board()
    {
        grid.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, 'O'));
    }
};

class Shot
{
public:
    char lastShot;
    int coordX;
    int coordY;

public:
    Shot(char lastShot, int coordX, int coordY)
        : lastShot(lastShot), coordX(coordX), coordY(coordY)
    {
    }

    Shot(const char *response)
    {
        // Buscar la primera coma
        const char *comma1 = strchr(response, ',');
        if (comma1 == nullptr)
        {
            // Si no se encuentra ninguna coma, salir o manejar el error según corresponda
            return;
        }

        // Calcular la posición de la primera coma
        size_t position1 = comma1 - response;

        // Extraer el último tiro
        lastShot = response[0];

        // Extraer la coordenada X (después de la primera coma)
        coordX = atoi(response + 1);

        // Extraer la coordenada Y (después de la segunda coma)
        coordY = atoi(response + position1 + 1);
    }

    char getLastShot() const
    {
        return lastShot;
    }

    int getCoordX() const
    {
        return coordX;
    }

    int getCoordY() const
    {
        return coordY;
    }

    void setCoords(int x, int y)
    {
        coordX = x;
        coordY = y;
    }

    void setShot(const char *response)
    {
        int x, y;
        char serverShot;
        sscanf(response, "%c,%d,%d", &serverShot, &x, &y);
        lastShot = serverShot;
        if (lastShot == 'O')
        {
            lastShot = 'X';
        }
        coordY = y;
        coordX = x;
    }
};
Shot globalShot('*', -1, -1);

class BattleshipGame
{
public:
    Board clientBoard;
    Board serverBoard;
    char lastShot;

public:
    BattleshipGame()
    {
        lastShot = '*';
    }

    void run()
    {
        createBoard();
    }

public:
    void createBoard()
    {
        clientBoard = Board();
        serverBoard = Board();

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

            for (int i = 0; i < 100; i++)
            {
                Ship ship = createShip(type, size);

                if (isShipPositionValid(clientBoard, ship))
                {
                    addShipToBoard(clientBoard, ship);
                    break;
                }
            }
        }
    }

    Ship createShip(char type, int size)
    {
        Ship ship(type);
        int orientation = rand() % 2;

        int startX = rand() % BOARD_SIZE;
        int startY = rand() % BOARD_SIZE;

        for (int i = 0; i < size; i++)
        {
            int x = startX;
            int y = startY;

            if (orientation == 0)
            {
                y += i;
            }
            else
            {
                x += i;
            }

            Position position(x, y);
            ship.positions.push_back(position);
        }

        return ship;
    }

    bool isPositionValid(const Position &position)
    {
        return position.x >= 0 && position.x < BOARD_SIZE && position.y >= 0 && position.y < BOARD_SIZE;
    }

    bool isShipPositionValid(const Board &board, const Ship &ship)
    {
        for (const Position &position : ship.positions)
        {
            if (!isPositionValid(position) || board.grid[position.x][position.y] != 'O')
            {
                return false;
            }
        }
        return true;
    }

    void addShipToBoard(Board &board, const Ship &ship)
    {
        for (const Position &position : ship.positions)
        {
            board.grid[position.x][position.y] = ship.type;
            board.ships.push_back(ship);
        }
    }

    std::string boardToString(const Board &board)
    {
        std::string result;
        for (const auto &row : board.grid)
        {
            for (char cell : row)
            {
                result += cell;
            }
            result += ',';
        }

        // Eliminar la última coma si existe
        if (!result.empty())
        {
            result.pop_back();
        }

        return result;
    }

    void printBoard(const Board &tablero1, const Board &tablero2)
    {

        std::string letras = "ABCDEFGHIJKLMNO";
        std::string numeros = "  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15";

        // Imprimir encabezado
        std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
        std::cout << "  " << numeros << "   " << numeros << std::endl;

        // Imprimir filas
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            char letra = 'A' + i;
            // agregar un espacio antes de los números menores a 10 para que se vea mejor
            std::cout << letra << "   ";

            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (tablero1.grid[i][j] == 'X')
                {
                    std::cout << ANSI_RED << tablero1.grid[i][j] << ANSI_RESET << "  ";
                }
                else if (tablero1.grid[i][j] == 'O')
                {
                    std::cout << ANSI_BLUE << tablero1.grid[i][j] << ANSI_RESET << "  ";
                }
                else
                {
                    std::cout << tablero1.grid[i][j] << "  ";
                }
            }

            std::cout << "    ";

            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (tablero2.grid[i][j] == 'X')
                {
                    std::cout << ANSI_RED << tablero2.grid[i][j] << ANSI_RESET << "  ";
                }
                else if (tablero2.grid[i][j] == 'O')
                {
                    std::cout << ANSI_BLUE << tablero2.grid[i][j] << ANSI_RESET << "  ";
                }
                else
                {
                    std::cout << tablero2.grid[i][j] << "  ";
                }
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    void printBoards()
    {

        printBoard(clientBoard, serverBoard);
        std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
        cout << ANSI_GREEN << "Por favor, ingresa una coordenada válida en el siguiente formato:" << endl;
        cout << "Letra de la " << ANSI_CYAN << "A" << ANSI_GREEN << " a la " << ANSI_CYAN << "O" << ANSI_GREEN << " seguida de un número del " << ANSI_CYAN << "1" << ANSI_GREEN << " al " << ANSI_CYAN << "15" << ANSI_GREEN << " (ejemplo: " << ANSI_CYAN << "A1, J13" << ANSI_GREEN << ")." << ANSI_RESET << endl;
    }

    char getLetter(int n)
    {
        return 'A' + n;
    }

    char updateClientCell(int x, int y)
    {
        char oldValue = clientBoard.grid[x][y];
        clientBoard.grid[x][y] = 'X';
        return oldValue;
    }
    char updateServerCell(int x, int y, char value)
    {
        if (value == 'O')
        {
            value = 'X';
        }
        serverBoard.grid[x][y] = value;
        return value;
    }

    bool areAllShipsSunk()
    {
        for (const Ship &ship : clientBoard.ships)
        {
            for (const Position &position : ship.positions)
            {
                if (clientBoard.grid[position.x][position.y] != 'X')
                {
                    return false;
                }
            }
        }
        return true;
    }
};

BattleshipGame game;

#endif // CLASS_H
