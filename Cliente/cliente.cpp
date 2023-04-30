#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

// constantes del juego
const int BOARD_SIZE = 15;
const int NUM_SHIPS = 9; // 1 portaaviones, 2 buques, 2 submarinos, 3 lanchas
//  Portaaviones:
//  5 posiciones
//  Buque:
//  4 posiciones
//  Submarino:
//  3 posiciones
//  Lancha:
//  1 posiciones

// * Definicion de funciones

bool startGame( int);
char shoot(std::string, int);
void initializeBoards( char[BOARD_SIZE][BOARD_SIZE], char[BOARD_SIZE][BOARD_SIZE]);

// * Structuras

struct Ship
{
    char type; // 'P' = portaaviones, 'B' = buque, 'S' = submarino, 'L' = lancha
    int size;
};

// ! Main
int main()
{

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
            if(startGame(client_sockfd)){
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

    // ! coordenada y = [0] letra
    // ! coordenada x = [1] numero

    // * CREACION DE TABLERO
    char clientBoard[BOARD_SIZE][BOARD_SIZE];
    char serverBoard[BOARD_SIZE][BOARD_SIZE];
    initializeBoards(clientBoard, serverBoard);

    while (true)
    {
        cout << "Ingrese las coordenadas a disparar: " << endl;
        cin >> input;

        if (input == "exit")
        {
            return true;
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
            cout << "Le has dado a un barco enemigo: "<< resultado << endl;
        }
    }
    return (resultado);
}

// ! placeShip
void placeShip(char board[BOARD_SIZE][BOARD_SIZE], Ship ship)
{
    int shipSize = ship.size;
    int row, col, direction;
    bool isValid = true;

    do
    {
        // Generar una posición aleatoria para el barco
        row = rand() % BOARD_SIZE;
        col = rand() % BOARD_SIZE;

        // Generar una dirección aleatoria (0 = horizontal, 1 = vertical)
        direction = rand() % 2;

        // Verificar si es posible colocar el barco en esa posición y dirección
        for (int i = 0; i < shipSize; i++)
        {
            if (direction == 0)
            {
                if (col + i >= BOARD_SIZE || board[row][col + i] != 'O')
                {
                    isValid = false;
                    break;
                }
            }
            else
            {
                if (row + i >= BOARD_SIZE || board[row + i][col] != 'O')
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
                if (direction == 0)
                {
                    board[row][col + i] = ship.type;
                }
                else
                {
                    board[row + i][col] = ship.type;
                }
            }
        }
    } while (!isValid);
}

// ! initializeBoards
void initializeBoards(char clientBoard[BOARD_SIZE][BOARD_SIZE], char serverBoard[BOARD_SIZE][BOARD_SIZE])
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
    Ship portaaviones = {'P', 5};
    Ship buque = {'B', 4};
    Ship submarino = {'S', 3};
    Ship lancha = {'L', 1};

    placeShip(clientBoard, portaaviones);
    placeShip(clientBoard, buque);
    placeShip(clientBoard, submarino);
    placeShip(clientBoard, lancha);
}

void printBoard(const char board[BOARD_SIZE][BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}
