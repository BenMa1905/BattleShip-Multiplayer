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

#include "server.h"

using namespace std;

// Función para manejar la jugada del jugador
char handle_play(int client_sockfd)
{
    // Código para manejar la jugada del jugador
    cout << "Conexion establecida con: " << client_sockfd << endl;

    // Recibir el string del cliente
    string input;
    char buffer[3]; // Un buffer temporal para almacenar los caracteres recibidos
    if (recv(client_sockfd, buffer, sizeof(buffer) - 1, 0) < 0)
    {
        cerr << "Error al recibir el string del cliente" << endl;
        exit(EXIT_FAILURE);
    }
    buffer[2] = '\0'; // Asegurar que el buffer esté terminado en nulo
    input = buffer;

    // Mostrar el string recibido
    cout << "String recibido: " << input << endl;

    // Procesar el string y determinar el resultado del disparo
    char resultado = 'X'; // Ejemplo: El resultado del disparo es 'X'

    // Enviar el resultado al cliente
    if (send(client_sockfd, &resultado, sizeof(resultado), 0) < 0)
    {
        cerr << "Error al enviar el resultado al cliente" << endl;
        exit(EXIT_FAILURE);
    }

    // Devolver el resultado
    return resultado;
}

// funcion para controlar el usuario
void handle_user(int client_sockfd)
{

    // Declarar el tablero del servidor
    Board serverBoard;
    // Generar el tablero del servidor
    generateBoard(serverBoard);

    while (true)
    {
        char buffer[1024];
        int bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);

        if (bytes < 0)
        {
            cerr << "Error al recibir el mensaje" << endl;
            exit(EXIT_FAILURE);
        }

        // procesar el mensaje
        string message = string(buffer, bytes);
        cout << "Mensaje recibido: " << message << endl;

        // enviar un mensaje de respuesta
        string response = "Hola cliente";
        send(client_sockfd, response.c_str(), response.length(), 0);

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

    // * está creando un socket del tipo TCP/IP
    // *(conexión orientada a la transmisión de datos)
    // * utilizando direcciones IPv4 y el protocolo predeterminado.
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

            handle_user(client_sockfd);

            // close(client_sockfd);
            // exit(EXIT_SUCCESS);
        }
    }

    close(server_sockfd);
    return 0;
}