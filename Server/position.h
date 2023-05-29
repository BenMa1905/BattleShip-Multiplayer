#ifndef POSITION_H
#define POSITION_H

const int BOARD_SIZE = 15;

#include <iostream>
#include<ctime>
#include<cstdlib>

// clase que representa una posicion en el tablero
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

    Position()
    {
        this->x = -1;
        this->y = -1;
    }

    

    /**
     * Determina si una posición es válida dentro del tablero
     *
     * @brief Función para determinar si una posición es válida
     * @param position Posición a evaluar
     * @return true si la posición es válida, false en caso contrario
     */
    bool isValidPosition() const
    {
        return this->x >= 0 && this->x < BOARD_SIZE && this->y >= 0 && this->y < BOARD_SIZE;
    }

    
};

/**
 * funcion que genera una posicion aleatorio
 * @return Position posicion aleatoria
 */
Position randomPosition()
{
    // std::srand(std::time(nullptr));
    int x = rand() % BOARD_SIZE;
    int y = rand() % BOARD_SIZE;
    return Position(x, y);
}

#endif // POSITION_H