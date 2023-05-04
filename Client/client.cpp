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
    std::string resultado;

    // * CREACION DE TABLERO
    Board clientBoard = create_board();
    std::string boardString = board_to_string(clientBoard);
    if (send(client_sockfd, boardString.c_str(), boardString.length(), 0) < 0)
    {
        cerr << "Error al enviar el tablero del cliente" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        if (recv(client_sockfd, &resultado, sizeof(resultado), 0) < 0)
        {
            cerr << "Error al recibir turno" << endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            char result;
            if (resultado == "0")//si es 0 es el turno del cliente
            {
                result = manage_game(client_sockfd, '*');
            }
            else//si es 1 es el turno del servidor
            {
                char serverShot;
                print_board(clientBoard);
                cout << "Esperando turno del servidor" << endl;
                serverShot = receive_shot(client_sockfd);
                result = manage_game(client_sockfd, serverShot);
            }
        }
        // * FIN DEL JUEGO
        // TODO: añadir mensaje de quien comienza el turno
        // TODO: Falta el manejo de la respuesta como :
        // * D: Derrota del cliente
        // * V: Victoria del cliente
        // * E: Error
    }

    return false;
}
