#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

// Dependencias del juego
#include "server.h"
#include "game.h"

using namespace std;

// Función para manejar la jugada del jugador
void handle_play(int client_sockfd)
{
    // Código para manejar la jugada del jugador
    cout << "Conexion establecida con: " << client_sockfd << endl;
}

// funcion para controlar el usuario
void handle_user(int client_sockfd, pid_t pid)

{
    // definicion de variables para el juego
    Game game;
    game.id = pid;
    game.turn = 0;
    game.is_game_over = false;

    // inicializar el tablero
    Board player_board = create_board();
    Board server_board = create_board();

    // enviar un mensaje de respuesta
    string response = "inicializando juego\n";
    send(client_sockfd, response.c_str(), response.length(), 0);

    
    

    while (true)
    {
        char buffer[1024];
        int bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);

        if (bytes < 0)
        {
            cerr << "Error al recibir el mensaje de: " << client_sockfd << " pid: " << pid << endl;
            exit(EXIT_FAILURE);
            close(client_sockfd); // TODO: comprobar si es necesario cerrar el socket
        }

        // procesar el mensaje
        string message = string(buffer, bytes);
        cout << "\nMensaje recibido de " << client_sockfd << " pid: " << pid << "\nMensaje: " << message << endl;

        // cerrar la conexion si el cliente envia "exit"
        if (message == "exit")
        {
            break;
        }
    }
    close(client_sockfd);
    exit(EXIT_SUCCESS);
}

int main()
{
    // Se crea el socket del servidor
    int server_port = find_port(5000);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
    {
        cerr << "Error al crear el socket del servidor" << endl;
        exit(EXIT_FAILURE);
    }

    // Se configura la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Se vincula el socket a la dirección del servidor
    if (bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error al vincular el socket del servidor a la dirección" << endl;
        exit(EXIT_FAILURE);
    }

    // Se pone el socket en modo de escucha
    if (listen(server_sockfd, 5) < 0)
    {
        cerr << "Error al poner el socket en modo de escucha" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Servidor iniciado en el puerto " << ntohs(server_address.sin_port) << endl;

    while (true)
    {
        // Se acepta una conexión entrante de un cliente
        int client_sockfd = accept(server_sockfd, NULL, NULL);
        if (client_sockfd < 0)
        {
            cerr << "Error al aceptar la conexión entrante del cliente" << endl;
            continue;
        }

        // Se crea un nuevo proceso para manejar la jugada del jugador
        pid_t pid = fork();
        cout << "pid de conexion: " << pid << endl;
        if (pid < 0)
        {
            cerr << "Error al crear el proceso hijo" << endl;
            close(client_sockfd);
            continue;
        }
        else if (pid == 0)
        {
            // Este es el proceso hijo
            close(server_sockfd);

            handle_user(client_sockfd, pid);

            // close(client_sockfd);
            // exit(EXIT_SUCCESS);
        }
    }

    close(server_sockfd);
    return 0;
}