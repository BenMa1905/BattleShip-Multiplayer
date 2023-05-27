#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include <iostream>

#include "position.h"

const char SHIP_TYPES[] = {'P', 'B', 'B', 'S', 'S', 'L', 'L', 'L'};
const int SHIP_SIZES[] = {5, 4, 4, 3, 3, 1, 1, 1};
const int NUM_SHIPS = 8;
// clase que representa un barco
class Ship
{
public:
    std::vector<Position> positions;
    std::vector<bool> hits; // indica si el barco ha sido tocado en esa posicion
    char type;              // 'P' = portaaviones, 'B' = buque, 'S' = submarino, 'L' = lancha, 'X' = hundido, 'O' = agua

    Ship()
    {
        this->type = 'O';
    }

    Ship(char type, int size)
    {
        this->type = type;
        this->positions = std::vector<Position>(size);
        this->hits = std::vector<bool>(size, false);
    }
    
    // constructor para crear un barco sin sabler sus posiciones
    Ship(char type)
    {
        this->type = type;
        int size = 0;
        for (int i = 0; i < sizeof(SHIP_TYPES); i++)
        {
            if (SHIP_TYPES[i] == type)
            {
                size = SHIP_SIZES[i];
                break;
            }
        }

        this->positions = std::vector<Position>(size);
        this->hits = std::vector<bool>(size, false);
    }

    void setPosition(int index, int x, int y)
    {
        this->positions[index].x = x;
        this->positions[index].y = y;
    }

    void setType(char type)
    {
        this->type = type;
    }

    /**
     * Establece si el barco ha sido tocado en una posición
     *
     * @brief Establece si el barco ha sido tocado en una posición
     * @param index Índice de la posición
     * @param hit true si el barco ha sido tocado en esa posición, false en caso contrario
     * @return void
     */
    void setHit(int index, bool hit)
    {
        this->hits[index] = hit;
    }

    /**
     * Comprueba si el barco ha sido hundido
     *
     * @brief Comprueba si el barco ha sido hundido
     * @return true si el barco ha sido hundido, false en caso contrario
     */
    bool isSunk() const
    {
        for (int i = 0; i < hits.size(); i++)
        {
            if (!hits[i])
            {
                return false;
            }
        }
        return true;
    }
};




#endif // SHIP_H
