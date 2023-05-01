#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctime>

using namespace std;

#include "client.h"

// * Definicion de funciones

bool startGame(int);
void initializeBoards();
Ship placeShip(int, char);

// ! Main
int main()
{
    // Se inicializa la semilla para generar números aleatorios
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    // * INICIO DEL PROGRAMA
    // Se crea el socket del cliente
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd < 0)
    {
        cerr << "Error al abrir el socket del cliente" << endl;
        exit(EXIT_FAILURE);
    }

    // Se especifica la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5000);                   // Puerto del servidor
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección IP del servidor

    // Se establece la conexión con el servidor
    if (connect(client_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error al establecer la conexión con el servidor" << endl;
        exit(EXIT_FAILURE);
    }

    // * INICIO DEL JUEGO

    do
    {
        cout << "Bienvenido al juego de batalla naval" << endl;
        cout << "1. Iniciar partida" << endl;
        cout << "2. Salir" << endl;
        cout << "Ingrese una opción: ";
        int option;
        cin >> option;
        switch (option)
        {
        case 1:
            cout << "Iniciando partida..." << endl;
            if (startGame(client_sockfd))
            {
                close(client_sockfd);
                return 0;
            };
            break;
        case 2:
            cout << "Saliendo del juego..." << endl;
            // Se cierra la conexión y el socket del cliente
            close(client_sockfd);
            return 0;
        default:
            cout << "Opción inválida" << endl;
            break;
        }
    } while (true);
    cout << "Saliendo del juego..." << endl;
    // Se cierra la conexión y el socket del cliente
    close(client_sockfd);
    return 0;

    return 0;
}

// ! startGame
bool startGame(int client_sockfd)
{
    // * DECLARACION DE VARIABLES
    std::string input;
    cout << "Creando input" << endl;

    // * CREACION DE TABLERO
    cout << "Creando tablero" << endl;
    initializeBoards();
    send(client_sockfd, clientBoard, sizeof(clientBoard), 0);

    cout << "Tablero creado" << endl;
    cout << "Renderizando tablero" << endl;

    printBoard(clientBoard, serverBoard);

    while (true)
    {
        cout << "Ingrese las coordenadas a disparar: " << endl;
        cin >> input;

        if (input == "exit")
        {
            return false;
        }
        // Verificar que el string tenga exactamente 2 caracteres
        if (input.length() != 2)
        {
            std::cout << "El string debe tener exactamente 2 caracteres." << std::endl;
            continue;
        }

        // Verificar que el primer caracter sea una letra y el segundo sea un número
        if (!isalpha(input[0]) || !isdigit(input[1]))
        {
            std::cout << "El primer caracter debe ser una letra y el segundo un número." << std::endl;
            continue;
        }
        // Convertir la letra a mayúscula
        input[0] = toupper(input[0]);

        shoot(input, client_sockfd);
    }
    return false;
}



// ! initializeBoards
void initializeBoards()
{
    // Inicializar los tableros con 'O'
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            clientBoard[i][j] = 'O';
            serverBoard[i][j] = 'O';
        }
    }

    // Generar los barcos para el cliente
    Ship portaaviones = placeShip(5, 'P');
    Ship buque[2];
    Ship submarino[2];
    for (int i = 0; i < 2; i++)
    {
        buque[i] = placeShip(4, 'B');
        submarino[i] = placeShip(3, 'S');
    }
    Ship lancha[5];
    for (int i = 0; i < 3; i++)
    {
        lancha[i] = placeShip(1, 'L');
    }
}

// ! placeShip
Ship placeShip(int shipSize, char identifier)
{
    cout << "Colocando barco de tamaño " << shipSize << endl;
    int row, col, direction;
    bool isValid = true;
    std::vector<Position> shipPositions;

    do
    {
        isValid = true;
        // Generar una posición aleatoria para el barco
        row = rand() % BOARD_SIZE;
        col = rand() % BOARD_SIZE;

        // Generar una dirección aleatoria (0 = horizontal, 1 = vertical)
        direction = rand() % 2;
        cout << "Dirección: " << direction << endl;
        if (direction == 0)
        {
            cout << "Dirección: horizontal" << endl;
        }
        else
        {
            cout << "Dirección: vertical" << endl;
        }
        // Verificar si es posible colocar el barco en esa posición y dirección
        for (int i = 0; i < shipSize; i++)
        {
            if (direction == 0)
            {
                if (col + i >= BOARD_SIZE || clientBoard[row][col + i] != 'O')
                {
                    isValid = false;
                    break;
                }
            }
            else
            {
                if (row + i >= BOARD_SIZE || clientBoard[row + i][col] != 'O')
                {
                    isValid = false;
                    break;
                }
            }
        }

        // Si es válido, colocar el barco en la posición y dirección
        if (isValid)
        {
            for (int i = 0; i < shipSize; i++)
            {
                Position pos;
                if (direction == 0)
                {
                    clientBoard[row][col + i] = identifier;
                    pos.x = row;
                    pos.y = col + i;
                }
                else
                {
                    clientBoard[row + i][col] = identifier;
                    pos.x = row + i;
                    pos.y = col;
                }
                shipPositions.push_back(pos);
                isValid = true;
            }
        }
    } while (!isValid);

    cout << "Barco colocado" << endl;

    return {shipPositions, identifier};
}
